#if !defined(__ANDROID__)
#include "Spatializer.h"
#include "HephException.h"
#include "File.h"
#include "FloatBuffer.h"
#include "Fourier.h"
#include "AudioProcessor.h"
#include "Windows/HannWindow.h"
#include "ConsoleLogger.h"
#include <cmath>
#include <vector>

#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(lib, "libmysofa/lib/windows/msvc/mysofa.lib")
#pragma comment(lib, "libmysofa/lib/windows/zlib.lib")
#endif

using namespace HephCommon;

namespace HephAudio
{
	Spatializer::Spatializer() : pEasy(nullptr), frameCount(0), sampleRate(48000)
	{
		// check the possible paths to find the default sofa file
		// if found, initialize with it.
		if (HephCommon::File::FileExists("dependencies/libmysofa/default.sofa"))
		{
			this->OpenSofaFile("HephAudio/dependencies/libmysofa/default.sofa", this->sampleRate);
		}
		else if (HephCommon::File::FileExists("../dependencies/libmysofa/default.sofa"))
		{
			this->OpenSofaFile("../dependencies/libmysofa/default.sofa", this->sampleRate);
		}
		else if (HephCommon::File::FileExists("../../dependencies/libmysofa/default.sofa"))
		{
			this->OpenSofaFile("../../dependencies/libmysofa/default.sofa", this->sampleRate);
		}
		else if (HephCommon::File::FileExists("HephAudio/dependencies/libmysofa/default.sofa"))
		{
			this->OpenSofaFile("HephAudio/dependencies/libmysofa/default.sofa", this->sampleRate);
		}
		else
		{
			HEPHAUDIO_LOG("Could not find the default sofa file. Provide one before proceeding.", HEPH_CL_WARNING);
		}
	}
	Spatializer::Spatializer(const std::string& sofaFilePath, uint32_t sampleRate) : pEasy(nullptr), frameCount(0), sampleRate(sampleRate)
	{
		this->OpenSofaFile(sofaFilePath, sampleRate);
	}
	Spatializer::Spatializer(Spatializer&& rhs) noexcept : pEasy(rhs.pEasy), frameCount(rhs.frameCount), sampleRate(rhs.sampleRate)
	{
		rhs.pEasy = nullptr;
	}
	Spatializer::~Spatializer()
	{
		this->CloseSofaFile();
	}
	Spatializer& Spatializer::operator=(Spatializer&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->CloseSofaFile();

			this->pEasy = rhs.pEasy;
			this->frameCount = rhs.frameCount;
			this->sampleRate = rhs.sampleRate;

			rhs.pEasy = nullptr;
		}

		return *this;
	}
	uint32_t Spatializer::GetSampleRate() const
	{
		return this->sampleRate;
	}
	void Spatializer::OpenSofaFile(const std::string& sofaFilePath, uint32_t sampleRate)
	{
		int filter_length;
		int errorCode;

		this->pEasy = mysofa_open(sofaFilePath.c_str(), sampleRate, &filter_length, &errorCode);
		if (this->pEasy == nullptr || errorCode != MYSOFA_OK)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "Spatializer::OpenSofaFile", "An error occurred while opening the sofa file.", "libmysofa", Spatializer::GetErrorString(errorCode)));
			this->CloseSofaFile();
			return;
		}
		this->frameCount = filter_length;
	}
	void Spatializer::CloseSofaFile()
	{
		if (this->pEasy != nullptr)
		{
			mysofa_close(this->pEasy);
			this->pEasy = nullptr;
		}
	}
	void Spatializer::Process(AudioBuffer& buffer, float azimuth_deg, float elevation_deg)
	{
		HannWindow hannWindow(this->frameCount);
		this->Process(buffer, azimuth_deg, elevation_deg, hannWindow.GenerateBuffer());
	}
	void Spatializer::Process(AudioBuffer& buffer, float azimuth_deg, float elevation_deg, const FloatBuffer& windowBuffer)
	{
		if (this->pEasy == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Spatializer::Process", "Read a sofa file before calling this method."));
			return;
		}

		if (windowBuffer.FrameCount() != this->frameCount)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Spatializer::Process", "Size of the window buffer must be the same as the IR size. Call \"Spatializer::GetFrameCount()\" method to get the IR size."));
			return;
		}

		float cartesian[3] = { -azimuth_deg, elevation_deg, this->pEasy->lookup->radius_min };
		mysofa_s2c(cartesian);

		ComplexBuffer leftTF(this->frameCount);
		ComplexBuffer rightTF(this->frameCount);
		{
			std::vector<float> leftIR(this->frameCount);
			std::vector<float> rightIR(this->frameCount);
			float delayLeft, delayRight;
			mysofa_getfilter_float(this->pEasy, cartesian[0], cartesian[1], cartesian[2], &leftIR[0], &rightIR[0], &delayLeft, &delayRight);

			for (size_t i = 0; i < this->frameCount; i++)
			{
				leftTF[i].real(leftIR[i]);
				rightTF[i].real(rightIR[i]);
			}

			Fourier::FFT(leftTF, this->frameCount);
			Fourier::FFT(rightTF, this->frameCount);
		}

		const size_t hopSize = this->frameCount / 4;

		AudioProcessor::ChangeChannelLayout(buffer, HEPHAUDIO_CH_LAYOUT_STEREO);
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);
		buffer.Reset();

		for (size_t i = 0; i < buffer.FrameCount(); i += hopSize)
		{
			ComplexBuffer left = leftTF * Fourier::FFT(channels[0].GetSubBuffer(i, this->frameCount) * windowBuffer, this->frameCount);
			ComplexBuffer right = rightTF * Fourier::FFT(channels[1].GetSubBuffer(i, this->frameCount) * windowBuffer, this->frameCount);

			Fourier::IFFT(left, false);
			Fourier::IFFT(right, false);

			for (size_t j = 0, k = i; j < this->frameCount && k < buffer.FrameCount(); j++, k++)
			{
				buffer[k][0] += left[j].real() * windowBuffer[j] / this->frameCount;
				buffer[k][1] += right[j].real() * windowBuffer[j] / this->frameCount;
			}
		}
	}
	std::string Spatializer::GetErrorString(int errorCode)
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
#endif