#include "RoomImpulseResponse.h"
#include "HephException.h"
#include "HephMath.h"
#include "Fourier.h"

using namespace HephCommon;

namespace HephAudio
{
	RoomImpulseResponse::RoomImpulseResponse(uint32_t sampleRate, Vector3 roomSize, heph_float c,
		heph_float frequencyAbsorptionCoefficients[][surfaceCount + 1], size_t nFrequency)
		: sampleRate(sampleRate), roomSize(roomSize), c(c), Vroom(roomSize.x* roomSize.y* roomSize.z)
		, frequencies(nFrequency), RT60(nFrequency), BX1(nFrequency), BX2(nFrequency), BY1(nFrequency), BY2(nFrequency), BZ1(nFrequency), BZ2(nFrequency)
		, wallXZ(roomSize.x* roomSize.z), wallYZ(roomSize.y* roomSize.z), wallXY(roomSize.x* roomSize.y)
	{
		for (size_t i = 0; i < nFrequency; i++)
		{
			this->frequencies[i] = frequencyAbsorptionCoefficients[i][0];

			this->BX1[i] = sqrt(1.0 - frequencyAbsorptionCoefficients[i][1]); // calculate the frequency dependant reflection coefficients for each wall
			this->BX2[i] = sqrt(1.0 - frequencyAbsorptionCoefficients[i][2]);
			this->BY1[i] = sqrt(1.0 - frequencyAbsorptionCoefficients[i][3]);
			this->BY2[i] = sqrt(1.0 - frequencyAbsorptionCoefficients[i][4]);
			this->BZ1[i] = sqrt(1.0 - frequencyAbsorptionCoefficients[i][5]);
			this->BZ2[i] = sqrt(1.0 - frequencyAbsorptionCoefficients[i][6]);

			const heph_float Sa =
				this->wallYZ * (frequencyAbsorptionCoefficients[i][1] + frequencyAbsorptionCoefficients[i][2]) +
				this->wallXZ * (frequencyAbsorptionCoefficients[i][3] + frequencyAbsorptionCoefficients[i][4]) +
				this->wallXY * (frequencyAbsorptionCoefficients[i][5] + frequencyAbsorptionCoefficients[i][6]);

			this->RT60[i] = (55.25 / this->c) * this->Vroom / Sa;
		}

		heph_float maxRT60 = -FLT_MAX;
		for (size_t i = 0; i < this->frequencies.FrameCount(); i++)
		{
			if (this->RT60[i] > maxRT60)
			{
				maxRT60 = this->RT60[i];
			}
		}
		this->impulseResponseRange = this->c * maxRT60;
	}
	FloatBuffer RoomImpulseResponse::SimulateRoomIR(Vector3 source, Vector3 reciever, size_t fftSize, Window& window, uint32_t imageRangeLimit) const
	{
		FloatBuffer impulseResponse;

		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const FloatBuffer windowBuffer = window.GenerateBuffer();

		const int32_t n_max = Math::Min((uint32_t)Math::Ceil(this->impulseResponseRange * 0.5 / this->roomSize.x), imageRangeLimit);
		const int32_t l_max = Math::Min((uint32_t)Math::Ceil(this->impulseResponseRange * 0.5 / this->roomSize.y), imageRangeLimit);
		const int32_t m_max = Math::Min((uint32_t)Math::Ceil(this->impulseResponseRange * 0.5 / this->roomSize.z), imageRangeLimit);

		const heph_float sourceXYZ[3][8] =
		{
			{ -source.x, -source.x, -source.x, -source.x, source.x, source.x, source.x, source.x },
			{ -source.y, -source.y, source.y, source.y, -source.y, -source.y, source.y, source.y },
			{ -source.z, source.z, -source.z, source.z, -source.z, source.z, -source.z, source.z }
		};
		const heph_float q[8] = { 0, 0, 0, 0, 1, 1, 1, 1 };
		const heph_float j[8] = { 0, 0, 1, 1, 0, 0, 1, 1 };
		const heph_float k[8] = { 0, 1, 0, 1, 0, 1, 0, 1 };

		FloatBuffer frequencies2(this->frequencies.FrameCount() + 2);
		frequencies2[0] = 0;
		for (size_t i = 0; i < this->frequencies.FrameCount(); i++)
		{
			frequencies2[i + 1] = this->frequencies[i];
		}
		frequencies2[frequencies2.FrameCount() - 1] = this->sampleRate * 0.5;

		for (int32_t n = -n_max; n <= n_max; n++)
		{
			for (int32_t l = -l_max; l <= l_max; l++)
			{
				for (int32_t m = -m_max; m <= m_max; m++)
				{
					for (uint32_t p = 0; p < 8; p++)
					{
						FloatBuffer sourceImagePower(this->frequencies.FrameCount());
						for (size_t i = 0; i < this->frequencies.FrameCount(); i++)
						{
							sourceImagePower[i] =
								pow(this->BX1[i], abs(n - q[p])) * pow(this->BY1[i], abs(l - j[p])) * pow(this->BZ1[i], abs(m - k[p])) *
								pow(this->BX2[i], abs(n)) * pow(this->BY2[i], abs(l)) * pow(this->BZ2[i], abs(m));
						}

						sourceImagePower.Resize(sourceImagePower.FrameCount() + 2);
						sourceImagePower >>= 1;
						sourceImagePower[0] = sourceImagePower[1];
						sourceImagePower[sourceImagePower.FrameCount() - 1] = sourceImagePower[sourceImagePower.FrameCount() - 2];

						ComplexBuffer sourceImageTransferFunction(fftSize);
						sourceImageTransferFunction[0].real = sourceImagePower[0];
						sourceImageTransferFunction[fftSize - 1] = sourceImageTransferFunction[0].Conjugate();
						const size_t nyquistFrequency = fftSize / 2;
						for (size_t i = 1; i < nyquistFrequency; i++)
						{
							const heph_float binFrequency = Fourier::IndexToBinFrequency(this->sampleRate, fftSize, i);

							heph_float f_lower = 0;
							heph_float f_upper = 0;
							size_t i_lower = 0;
							size_t i_upper = 0;
							for (size_t j = 0; j < frequencies2.FrameCount(); j++)
							{
								if (frequencies2[j] >= binFrequency)
								{
									f_upper = frequencies2[j];
									i_upper = j;
									break;
								}
								f_lower = frequencies2[j];
								i_lower = j;
							}
							const heph_float alpha = (binFrequency - f_lower) / (f_upper - f_lower);

							sourceImageTransferFunction[i].real = sourceImagePower[i_lower] * (1.0 - alpha) + sourceImagePower[i_upper] * alpha;
							sourceImageTransferFunction[fftSize - i - 1] = sourceImageTransferFunction[i].Conjugate();
						}

						FloatBuffer sourceImageImpulseRespnose(fftSize);
						Fourier::FFT_Inverse(sourceImageImpulseRespnose, sourceImageTransferFunction);
						
						const Vector3 sourceImage(2 * n * this->roomSize.x - sourceXYZ[0][p], 2 * l * this->roomSize.y - sourceXYZ[1][p], 2 * m * this->roomSize.z - sourceXYZ[2][p]);
						const size_t delay = this->sampleRate * sourceImage.Distance(reciever) / this->c;

						if (delay + sourceImageImpulseRespnose.FrameCount() >= impulseResponse.FrameCount())
						{
							impulseResponse.Resize(delay + sourceImageImpulseRespnose.FrameCount());
						}

						for (size_t i = 0; i < sourceImageImpulseRespnose.FrameCount(); i++)
						{
							impulseResponse[i + delay] += sourceImageImpulseRespnose[i] * window[i];
						}
					}
				}
			}
		}

		return impulseResponse;
	}
	ComplexBuffer RoomImpulseResponse::SimulateRoomTF(Vector3 source, Vector3 reciever, size_t fftSize, Window& window, uint32_t imageRangeLimit) const
	{
		const FloatBuffer impulseResponse = this->SimulateRoomIR(source, reciever, fftSize, window, imageRangeLimit);
		return Fourier::FFT_Forward(impulseResponse, Fourier::CalculateFFTSize(impulseResponse.FrameCount()));
	}
	FloatBuffer RoomImpulseResponse::GetFrequencies() const 
	{
		return this->frequencies;
	}
	FloatBuffer RoomImpulseResponse::GetRT60() const
	{
		return this->RT60;
	}
	void RoomImpulseResponse::GetWallReflectionCoefficients(FloatBuffer* pBX1, FloatBuffer* pBX2, FloatBuffer* pBY1, FloatBuffer* pBY2, FloatBuffer* pBZ1, FloatBuffer* pBZ2) const
	{
		if (pBX1 != nullptr)
		{
			(*pBX1) = this->BX1;
		}
		if (pBX2 != nullptr)
		{
			(*pBX2) = this->BX2;
		}
		if (pBY1 != nullptr)
		{
			(*pBY1) = this->BY1;
		}
		if (pBY2 != nullptr)
		{
			(*pBY2) = this->BY2;
		}
		if (pBZ1 != nullptr)
		{
			(*pBZ1) = this->BZ1;
		}
		if (pBZ2 != nullptr)
		{
			(*pBZ2) = this->BZ2;
		}
	}
	heph_float RoomImpulseResponse::GetImpulseResponseRange() const
	{
		return this->impulseResponseRange;
	}
}