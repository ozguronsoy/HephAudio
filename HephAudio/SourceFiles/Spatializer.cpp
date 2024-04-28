#include "Spatializer.h"
#include "HephException.h"
#include "File.h"
#include "FloatBuffer.h"
#include "Fourier.h"
#include "AudioProcessor.h"
#include "Windows/HannWindow.h"
#include "ConsoleLogger.h"
#include "../../dependencies/libsofa/src/SOFA.h"
#include <cmath>
#include <vector>

#if defined(_WIN32) && defined(_MSC_VER)

#if defined(HEPH_ENV_64_BIT)

#if defined(_DEBUG)
#pragma comment(lib, "libsofa/lib/win/libsofa_debug_x64.lib")
#else
#pragma comment(lib, "libsofa/lib/win/libsofa_x64.lib")
#endif

#pragma comment(lib, "libsofa/lib/win/netcdf_x64.lib")

#else

#if defined(_DEBUG)
#pragma comment(lib, "libsofa/lib/win/libsofa_debug.lib")
#else
#pragma comment(lib, "libsofa/lib/win/libsofa.lib")
#endif

#pragma comment(lib, "libsofa/lib/win/netcdf.lib")

#endif

#endif

#define SOFA_KEY_DELAY "Data.Delay"
#define SOFA_KEY_IR "Data.IR"
#define SOFA_KEY_REAL "Data.REAL"
#define SOFA_KEY_IMAG "Data.IMAG"
#define SOFA_KEY_SAMPLE_RATE "Data.SamplingRate"
#define SOFA_KEY_RECIEVER_POS "ReceiverPosition"
#define SOFA_KEY_SOURCE_POS "SourcePosition"

using namespace HephCommon;

namespace HephAudio
{
	Spatializer::Spatializer() : measurementCount(0), frameCount(0), sampleRate(0), deltaAzimuth_deg(0), deltaElevation_deg(0)
	{
		// check the possible paths to find the default sofa file
		// if found, initialize with it.
		if (HephCommon::File::FileExists("dependencies/libsofa/default.sofa"))
		{
			this->ReadSofaFile("HephAudio/dependencies/libsofa/default.sofa");
		}
		else if (HephCommon::File::FileExists("../dependencies/libsofa/default.sofa"))
		{
			this->ReadSofaFile("../dependencies/libsofa/default.sofa");
		}
		else if (HephCommon::File::FileExists("../../dependencies/libsofa/default.sofa"))
		{
			this->ReadSofaFile("../../dependencies/libsofa/default.sofa");
		}
		else if (HephCommon::File::FileExists("HephAudio/dependencies/libsofa/default.sofa"))
		{
			this->ReadSofaFile("HephAudio/dependencies/libsofa/default.sofa");
		}
		else
		{
			HEPHAUDIO_LOG("Could not find the default sofa file. Provide one before proceeding.", HEPH_CL_WARNING);
		}
	}
	Spatializer::Spatializer(const std::string& sofaFilePath) : Spatializer()
	{
		this->ReadSofaFile(sofaFilePath);
	}
	uint32_t Spatializer::GetSampleRate() const
	{
		return this->sampleRate;
	}
	void Spatializer::ReadSofaFile(const std::string& sofaFilePath)
	{
		this->windowBuffer.Release();
		std::vector<Spatializer::TransferFunctions>().swap(this->transferFunctions); // clear and realloc
		std::vector<Spatializer::SourcePosition>().swap(this->sourcePositions);
		this->sofaFilePath = sofaFilePath;

		if (!HephCommon::File::FileExists(sofaFilePath))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Spatializer::SetSofaFile", "File does not exist."));
			return;
		}

		if (!sofa::IsValidSOFAFile(sofaFilePath))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Spatializer::SetSofaFile", "File must be a sofa file."));
			return;
		}

		sofa::NetCDFFile file(sofaFilePath, netCDF::NcFile::FileMode::read);

		this->measurementCount = file.GetDimension("M");
		this->frameCount = file.GetDimension("N");

		const size_t recieverCount = file.GetDimension("R");
		if (recieverCount != 2)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Spatializer::SetSofaFile", "Invalid sofa file, number of recievers must be 2."));
			return;
		}

		// read sampling rate
		{
			std::vector<double> tempSampleRates;
			if (!file.GetValues(tempSampleRates, SOFA_KEY_SAMPLE_RATE) || tempSampleRates.size() == 0)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "Spatializer::ReadSofaFile", "Failed to read the sampling rate."));
				return;
			}
			this->sampleRate = tempSampleRates[0];
		}

		this->windowBuffer = HannWindow(this->frameCount).GenerateBuffer();

		// read source positions
		{
			std::vector<double> tempSourcePositions;
			if (!file.GetValues(tempSourcePositions, SOFA_KEY_SOURCE_POS))
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "Spatializer::ReadSofaFile", "Failed to read the source positions."));
				this->windowBuffer.Release();
				return;
			}

			this->sourcePositions.resize(this->measurementCount);
			for (size_t i = 0; i < this->measurementCount; i++)
			{
				this->sourcePositions[i].azimuth_deg = tempSourcePositions[i * 3 + 0];
				this->sourcePositions[i].elevation_deg = tempSourcePositions[i * 3 + 1];
			}
		}

		// find delta (grid) angles
		{
			double a_min = HEPHAUDIO_SPATIALIZER_AZIMUTH_MAX;
			double e_min = HEPHAUDIO_SPATIALIZER_ELEVATION_MAX;

			for (size_t i = 0; i < this->measurementCount; i++)
			{
				if (this->sourcePositions[i].azimuth_deg < a_min)
				{
					a_min = this->sourcePositions[i].azimuth_deg;
				}

				if (this->sourcePositions[i].elevation_deg < e_min)
				{
					e_min = this->sourcePositions[i].elevation_deg;
				}
			}

			this->deltaAzimuth_deg = HEPHAUDIO_SPATIALIZER_AZIMUTH_MAX;
			this->deltaElevation_deg = HEPHAUDIO_SPATIALIZER_ELEVATION_MAX;
			for (size_t i = 0; i < this->measurementCount; i++)
			{
				if (a_min != this->sourcePositions[i].azimuth_deg)
				{
					const double da = fabs(a_min - this->sourcePositions[i].azimuth_deg);
					if (da < this->deltaAzimuth_deg)
					{
						this->deltaAzimuth_deg = da;
					}
				}

				if (e_min != this->sourcePositions[i].elevation_deg)
				{
					const double de = fabs(e_min - this->sourcePositions[i].elevation_deg);
					if (de < this->deltaElevation_deg)
					{
						this->deltaElevation_deg = de;
					}
				}
			}
		}

		// read impulse responses
		if (file.HasVariable(SOFA_KEY_IR))
		{
			std::vector<double> tempIRs;
			if (!file.GetValues(tempIRs, SOFA_KEY_IR))
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "Spatializer::ReadSofaFile", "Failed to read the IR."));
				this->windowBuffer.Release();
				std::vector<Spatializer::SourcePosition>().swap(this->sourcePositions);
				return;
			}

			this->transferFunctions.resize(this->measurementCount);
			for (size_t i = 0, irStartIndex = 0; i < this->measurementCount; i++)
			{
				this->transferFunctions[i].left.Resize(this->frameCount);
				this->transferFunctions[i].right.Resize(this->frameCount);

				for (size_t j = 0; j < this->frameCount; j++)
				{
					this->transferFunctions[i].left[j] = Complex(tempIRs[irStartIndex + j], 0);
				}
				irStartIndex += this->frameCount;
				Fourier::FFT(this->transferFunctions[i].left);

				for (size_t j = 0; j < this->frameCount; j++)
				{
					this->transferFunctions[i].right[j] = Complex(tempIRs[irStartIndex + j], 0);
				}
				irStartIndex += this->frameCount;
				Fourier::FFT(this->transferFunctions[i].right);
			}
		}
		else
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Spatializer::ReadSofaFile", "Invalid sofa file! No IR found."));
			this->windowBuffer.Release();
			std::vector<Spatializer::SourcePosition>().swap(this->sourcePositions);
			return;
		}

		return;
	}
	void Spatializer::Process(AudioBuffer& buffer, double azimuth_deg, double elevation_deg)
	{
		if (this->transferFunctions.size() == 0 || this->sourcePositions.size() == 0)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_OPERATION, "Spatializer::Process", "Read a sofa file before calling this method."));
			return;
		}

		// wrap to [0, 360)
		azimuth_deg = fmod(azimuth_deg, 360);
		if (azimuth_deg < 0)
		{
			azimuth_deg += 360;
		}

		// wrap to [-90, 90]
		elevation_deg = fmod(elevation_deg, 360);
		if (elevation_deg == 180 || elevation_deg == -180)
		{
			elevation_deg = 0;
		}
		else if (elevation_deg < -90 || elevation_deg > 90)
		{
			const int sign = (elevation_deg > 180 || elevation_deg < -180) ? -1 : 1;
			elevation_deg = fmod(elevation_deg, 90);
			elevation_deg = elevation_deg < 0 ? (sign * -(90 + elevation_deg)) : (sign * (90 - elevation_deg));
		}

		const Spatializer::TransferFunctions interpolatedTF = this->InterpolateRectangular(azimuth_deg, elevation_deg);
		const size_t hopSize = this->frameCount / 4;

		AudioProcessor::ChangeChannelLayout(buffer, HEPHAUDIO_CH_LAYOUT_STEREO);
		std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);
		buffer.Reset();

		for (size_t i = 0; i < buffer.FrameCount(); i += hopSize)
		{
			ComplexBuffer left = interpolatedTF.left * Fourier::FFT(channels[0].GetSubBuffer(i, this->frameCount) * this->windowBuffer, this->frameCount);
			ComplexBuffer right = interpolatedTF.right * Fourier::FFT(channels[1].GetSubBuffer(i, this->frameCount) * this->windowBuffer, this->frameCount);

			Fourier::IFFT(left, false);
			Fourier::IFFT(right, false);

			for (size_t j = 0, k = i; j < this->frameCount && k < buffer.FrameCount(); j++, k++)
			{
				buffer[k][0] += left[j].real() * this->windowBuffer[j] / this->frameCount;
				buffer[k][1] += right[j].real() * this->windowBuffer[j] / this->frameCount;
			}
		}
	}
	Spatializer::TransferFunctions Spatializer::InterpolateRectangular(double azimuth_deg, double elevation_deg) const
	{
		constexpr double ae_initial = -1000;

		double a0 = ae_initial;
		double e0 = ae_initial;

		// find the closest initial angles
		{
			double a_min = -ae_initial;
			double e_min = -ae_initial;

			for (size_t i = 0; i < this->measurementCount; i++)
			{
				double a = this->sourcePositions[i].azimuth_deg;
				if (a <= azimuth_deg && a > a0)
				{
					a0 = a;
				}

				if (a < a_min)
				{
					a_min = a;
				}


				double e = this->sourcePositions[i].elevation_deg;
				if (e <= elevation_deg && e > e0)
				{
					e0 = e;
				}

				if (e < e_min)
				{
					e_min = e;
				}
			}

			if (a0 == ae_initial)
			{
				a0 = a_min;
			}

			if (e0 == ae_initial)
			{
				e0 = e_min;
			}
		}

		const double ca = fmod(azimuth_deg, this->deltaAzimuth_deg) / this->deltaAzimuth_deg;
		const double ce = fmod(elevation_deg, this->deltaElevation_deg) / this->deltaElevation_deg;

		const size_t i0 = this->FindMeasurementIndex(a0, e0);
		const size_t i1 = this->FindMeasurementIndex(a0 + this->deltaAzimuth_deg, e0);
		const size_t i2 = this->FindMeasurementIndex(a0 + this->deltaAzimuth_deg, e0 + this->deltaElevation_deg);
		const size_t i3 = this->FindMeasurementIndex(a0, e0 + this->deltaElevation_deg);

		Spatializer::TransferFunctions impulseResponses;
		impulseResponses.left.Resize(this->frameCount);
		impulseResponses.right.Resize(this->frameCount);

		const ComplexBuffer& c0l = i0 != Spatializer::INDEX_NOT_FOUND ? this->transferFunctions[i0].left : ComplexBuffer(this->frameCount);
		const ComplexBuffer& c0r = i0 != Spatializer::INDEX_NOT_FOUND ? this->transferFunctions[i0].right : ComplexBuffer(this->frameCount);
		const ComplexBuffer& c1l = i1 != Spatializer::INDEX_NOT_FOUND ? this->transferFunctions[i1].left : ComplexBuffer(this->frameCount);
		const ComplexBuffer& c1r = i1 != Spatializer::INDEX_NOT_FOUND ? this->transferFunctions[i1].right : ComplexBuffer(this->frameCount);
		const ComplexBuffer& c2l = i2 != Spatializer::INDEX_NOT_FOUND ? this->transferFunctions[i2].left : ComplexBuffer(this->frameCount);
		const ComplexBuffer& c2r = i2 != Spatializer::INDEX_NOT_FOUND ? this->transferFunctions[i2].right : ComplexBuffer(this->frameCount);
		const ComplexBuffer& c3l = i3 != Spatializer::INDEX_NOT_FOUND ? this->transferFunctions[i3].left : ComplexBuffer(this->frameCount);
		const ComplexBuffer& c3r = i3 != Spatializer::INDEX_NOT_FOUND ? this->transferFunctions[i3].right : ComplexBuffer(this->frameCount);

		for (size_t i = 0; i < this->frameCount; i++)
		{
			double coeff = 0;

			coeff = (1.0 - ca) * (1.0 - ce);
			impulseResponses.left[i] += coeff * c0l[i];
			impulseResponses.right[i] += coeff * c0r[i];

			coeff = ca * (1.0 - ce);
			impulseResponses.left[i] += coeff * c1l[i];
			impulseResponses.right[i] += coeff * c1r[i];

			coeff = ca * ce;
			impulseResponses.left[i] += coeff * c2l[i];
			impulseResponses.right[i] += coeff * c2r[i];

			coeff = (1.0 - ca) * ce;
			impulseResponses.left[i] += coeff * c3l[i];
			impulseResponses.right[i] += coeff * c3r[i];
		}

		return impulseResponses;
	}
	size_t Spatializer::FindMeasurementIndex(double azimuth_deg, double elevation_deg) const
	{
		for (size_t i = 0; i < this->measurementCount; i++)
		{
			if (this->sourcePositions[i].azimuth_deg == azimuth_deg && this->sourcePositions[i].elevation_deg == elevation_deg)
			{
				return i;
			}
		}
		return Spatializer::INDEX_NOT_FOUND;
	}
}