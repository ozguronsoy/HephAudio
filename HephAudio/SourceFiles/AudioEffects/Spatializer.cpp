#include "AudioEffects/Spatializer.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Exceptions/InvalidOperationException.h"
#include "Exceptions/NotFoundException.h"
#include "Exceptions/ExternalException.h"
#include "Fourier.h"
#include "ConsoleLogger.h"
#include <vector>

#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(lib, "libmysofa/lib/windows/msvc/mysofa.lib")
#pragma comment(lib, "libmysofa/lib/windows/zlib.lib")
#endif

using namespace Heph;

namespace HephAudio
{
	Spatializer::Spatializer()
		: FrequencyDomainEffect(),
		azimuth(0), elevation(0),
		pEasy(nullptr), hrtfSize(0), hrtfSampleRate(48000),
		updateTransferFunctions(true)
	{
		this->OpenDefaultFile();
	}

	Spatializer::Spatializer(size_t hopSize, const Window& wnd) : Spatializer(0, 0, hopSize, wnd) {}

	Spatializer::Spatializer(double azimuth, double elevation, size_t hopSize, const Window& wnd)
		: FrequencyDomainEffect(hopSize, wnd),
		azimuth(azimuth), elevation(elevation),
		pEasy(nullptr), hrtfSize(0), hrtfSampleRate(48000),
		updateTransferFunctions(true)
	{
		this->OpenDefaultFile();
	}

	Spatializer::Spatializer(const std::filesystem::path& filePath, uint32_t sampleRate, double azimuth, double elevation, size_t hopSize, const Window& wnd)
		: FrequencyDomainEffect(hopSize, wnd),
		azimuth(azimuth), elevation(elevation),
		pEasy(nullptr), hrtfSize(0), hrtfSampleRate(sampleRate),
		updateTransferFunctions(true)
	{
		this->OpenSofaFile(filePath);
	}

	Spatializer::~Spatializer()
	{
		this->CloseSofaFile();
	}

	std::string Spatializer::Name() const
	{
		return "Spatializer";
	}

	void Spatializer::Process(AudioBuffer& buffer, size_t startIndex, size_t frameCount)
	{
		if (buffer.FormatInfo().channelLayout != HEPHAUDIO_CH_LAYOUT_STEREO)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "the audio buffer must be stereo."));
		}

		if (this->updateTransferFunctions)
		{
			const size_t wndSize = this->GetWindowSize();
			this->transferFunctions[0].Resize(wndSize);
			this->transferFunctions[1].Resize(wndSize);

			float cartesian[3] = { -this->azimuth, this->elevation, this->pEasy->lookup->radius_min };
			mysofa_s2c(cartesian);

			std::vector<float> leftIR(this->hrtfSize);
			std::vector<float> rightIR(this->hrtfSize);
			float delayLeft, delayRight;
			mysofa_getfilter_float(this->pEasy, cartesian[0], cartesian[1], cartesian[2], &leftIR[0], &rightIR[0], &delayLeft, &delayRight);

			if (this->hrtfSize == wndSize)
			{
				for (size_t i = 0; i < this->hrtfSize; ++i)
				{
					this->transferFunctions[0][i] = Complex(leftIR[i], 0);
					this->transferFunctions[1][i] = Complex(rightIR[i], 0);
				}
			}
			else
			{
				const double ratio = (((double)this->hrtfSize) / ((double)wndSize));
				double j = 0;
				for (size_t i = 0; i < wndSize; ++i, j += ratio)
				{
					this->transferFunctions[0][i] = Complex(leftIR[j] * (1.0 - ratio) + leftIR[j + 1] * ratio, 0);
					this->transferFunctions[1][i] = Complex(rightIR[j] * (1.0 - ratio) + rightIR[j + 1] * ratio, 0);
				}
			}

			Fourier::FFT(this->transferFunctions[0], wndSize);
			Fourier::FFT(this->transferFunctions[1], wndSize);

			this->updateTransferFunctions = false;
		}

		FrequencyDomainEffect::Process(buffer, startIndex, frameCount);
	}

	void Spatializer::SetWindow(const Window& wnd)
	{
		FrequencyDomainEffect::SetWindow(wnd);
		this->updateTransferFunctions = true;
	}

	void Spatializer::OpenSofaFile(const std::filesystem::path& filePath)
	{
		if (this->pEasy != nullptr)
		{
			if (filePath != this->filePath)
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidOperationException(HEPH_FUNC, "another SOFA file is open, close it first to change files."));
			}
			return;
		}

		if (!std::filesystem::exists(filePath))
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, NotFoundException(HEPH_FUNC, "file not found."));
		}

		this->filePath = filePath;

		int filter_length = 0;
		int errorCode = 0;

		this->pEasy = mysofa_open(filePath.string().c_str(), (float)this->hrtfSampleRate, &filter_length, &errorCode);
		if (this->pEasy == nullptr || errorCode != MYSOFA_OK)
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, ExternalException(HEPH_FUNC, "An error occurred while opening the sofa file.", "libmysofa", this->GetErrorString(errorCode)));
		}
		this->hrtfSize = filter_length;
		this->updateTransferFunctions = true;
	}

	void Spatializer::CloseSofaFile()
	{
		if (this->pEasy != nullptr)
		{
			mysofa_close(this->pEasy);
			this->pEasy = nullptr;
		}
	}

	double Spatializer::GetAzimuth() const
	{
		return this->azimuth;
	}

	void Spatializer::SetAzimuth(double azimuth)
	{
		if (this->azimuth != azimuth)
		{
			this->azimuth = azimuth;
			this->updateTransferFunctions = true;
		}
	}

	double Spatializer::GetElevation() const
	{
		return this->elevation;
	}

	void Spatializer::SetElevation(double elevation)
	{
		if (this->elevation != elevation)
		{
			this->elevation = elevation;
			this->updateTransferFunctions = true;
		}
	}

	uint32_t Spatializer::GetSampleRate() const
	{
		return this->hrtfSampleRate;
	}

	void Spatializer::SetSampleRate(uint32_t sampleRate)
	{
		if (this->hrtfSampleRate != sampleRate)
		{
			this->hrtfSampleRate = sampleRate;

			if (this->pEasy != nullptr)
			{
				this->CloseSofaFile();
				this->OpenSofaFile(this->filePath);
				this->updateTransferFunctions = true;
			}
		}
	}

	size_t Spatializer::GetHrtfSize() const
	{
		return this->hrtfSize;
	}

	void Spatializer::ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount)
	{
		int64_t firstWindowStartIndex;
		if (this->currentIndex < this->pastSamples.FrameCount())
		{
			firstWindowStartIndex = ((int64_t)startIndex) - ((int64_t)this->currentIndex);
		}
		else
		{
			firstWindowStartIndex = ((int64_t)startIndex) - ((int64_t)this->pastSamples.FrameCount());
			firstWindowStartIndex = firstWindowStartIndex - (firstWindowStartIndex % this->hopSize) + this->hopSize;
		}

		const int64_t endIndex = startIndex + frameCount;
		const AudioFormatInfo& formatInfo = inputBuffer.FormatInfo();
		const size_t fftSize = this->wnd.Size();
		const size_t nyquistBin = fftSize / 2;
		const double overflowFactor = 1.0 / this->CalculateMaxNumberOfOverlaps();

		for (int64_t i = firstWindowStartIndex; i < endIndex; i += this->hopSize)
		{
			for (size_t j = 0; j < formatInfo.channelLayout.count; ++j)
			{
				ComplexBuffer channel(fftSize);
				for (int64_t k = 0, l = i;
					(k < (int64_t)fftSize) && (l < (int64_t)inputBuffer.FrameCount());
					++k, ++l)
				{
					if (l < 0)
					{
						if ((-l) <= (int64_t)this->pastSamples.FrameCount())
						{
							channel[k].real = this->pastSamples[this->pastSamples.FrameCount() + l][j] * this->wnd[k];
						}
					}
					else
					{
						channel[k].real = inputBuffer[l][j] * this->wnd[k];
					}
				}

				Fourier::FFT(channel);
				channel *= this->transferFunctions[j];
				Fourier::IFFT(channel, false);

				for (int64_t k = 0, l = i;
					(k < (int64_t)fftSize) && (l < (int64_t)outputBuffer.FrameCount());
					++k, ++l)
				{
					if (l >= (int64_t)startIndex)
					{
						outputBuffer[l][j] += channel[k].real * overflowFactor * this->wnd[k] / fftSize;
					}
				}
			}
		}
	}

	void Spatializer::OpenDefaultFile()
	{
		const std::vector<std::filesystem::path> possiblePaths =
		{
			"dependencies/libmysofa/default.sofa",
			"../dependencies/libmysofa/default.sofa",
			"../../dependencies/libmysofa/default.sofa",
			"HephAudio/default.sofa",
			"HephAudio/dependencies/libmysofa/default.sofa"
		};

		for (const std::filesystem::path& path : possiblePaths)
		{
			if (std::filesystem::exists(path))
			{
				this->OpenSofaFile(path);
				return;
			}
		}

		HEPHAUDIO_LOG("Could not find the default sofa file. Provide one before proceeding.", HEPH_CL_WARNING);
	}

	std::string Spatializer::GetErrorString(int errorCode) const
	{
		switch (errorCode)
		{
		case MYSOFA_OK:
			return "No error";
		case MYSOFA_INTERNAL_ERROR:
			return "Internal error";
		case MYSOFA_UNSUPPORTED_FORMAT:
			return "Unsupported format.";
		case MYSOFA_NO_MEMORY:
			return "Insufficient memory.";
		case MYSOFA_READ_ERROR:
			return "Read error.";
		case MYSOFA_INVALID_ATTRIBUTES:
			return "Invalid attributes.";
		case MYSOFA_INVALID_DIMENSIONS:
			return "Invalid dimensions.";
		case MYSOFA_INVALID_DIMENSION_LIST:
			return "Invalid dimension list.";
		case MYSOFA_INVALID_COORDINATE_TYPE:
			return "Invalid coordinate type.";
		case MYSOFA_ONLY_EMITTER_WITH_ECI_SUPPORTED:
			return "Only emitter with ECI supported.";
		case MYSOFA_ONLY_DELAYS_WITH_IR_OR_MR_SUPPORTED:
			return "Only delays with IR or MR are supported.";
		case MYSOFA_ONLY_THE_SAME_SAMPLING_RATE_SUPPORTED:
			return "Only the same sampling rate supported.";
		case MYSOFA_RECEIVERS_WITH_RCI_SUPPORTED:
			return "Receivers with RCI supported.";
		case MYSOFA_RECEIVERS_WITH_CARTESIAN_SUPPORTED:
			return "Receivers with cartesian supported.";
		case MYSOFA_INVALID_RECEIVER_POSITIONS:
			return "Invalid reciever positions.";
		case MYSOFA_ONLY_SOURCES_WITH_MC_SUPPORTED:
			return "Only sources with MC supported.";
		default:
			return "Unknown error.";
		}
	}
}