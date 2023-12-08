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

			this->BX1[i] = sqrt(1.0 - frequencyAbsorptionCoefficients[i][1]);
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
		constexpr uint32_t p_max = 8;
		constexpr heph_float q[p_max] = { 0, 0, 0, 0, 1, 1, 1, 1 };
		constexpr heph_float j[p_max] = { 0, 0, 1, 1, 0, 0, 1, 1 };
		constexpr heph_float k[p_max] = { 0, 1, 0, 1, 0, 1, 0, 1 };

		FloatBuffer roomImpulseResponse;
		fftSize = Fourier::CalculateFFTSize(fftSize);
		window.SetSize(fftSize);
		const FloatBuffer windowBuffer = window.GenerateBuffer();

		const int32_t n_max = Math::Min((uint32_t)ceil(this->impulseResponseRange * 0.5 / this->roomSize.x), imageRangeLimit);
		const int32_t l_max = Math::Min((uint32_t)ceil(this->impulseResponseRange * 0.5 / this->roomSize.y), imageRangeLimit);
		const int32_t m_max = Math::Min((uint32_t)ceil(this->impulseResponseRange * 0.5 / this->roomSize.z), imageRangeLimit);

		const Vector3 sourceReflections[p_max] =
		{
			Vector3(-source.x, -source.y, -source.z), Vector3(-source.x, -source.y, source.z), Vector3(-source.x, source.y, -source.z),
			Vector3(-source.x, source.y, source.z), Vector3(source.x, -source.y, -source.z), Vector3(source.x, -source.y, source.z),
			Vector3(source.x, source.y, -source.z), Vector3(source.x, source.y, source.z)
		};

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
					for (uint32_t p = 0; p < p_max; p++)
					{
						FloatBuffer imagePower(frequencies2.FrameCount());
						for (size_t i = 0; i < this->frequencies.FrameCount(); i++)
						{
							imagePower[i + 1] =
								pow(this->BX1[i], abs(n - q[p])) * pow(this->BY1[i], abs(l - j[p])) * pow(this->BZ1[i], abs(m - k[p])) *
								pow(this->BX2[i], abs(n)) * pow(this->BY2[i], abs(l)) * pow(this->BZ2[i], abs(m));
						}
						imagePower[0] = imagePower[1];
						imagePower[imagePower.FrameCount() - 1] = imagePower[imagePower.FrameCount() - 2];

						ComplexBuffer imageTransferFunction(fftSize);
						imageTransferFunction[0].real = imagePower[0];
						imageTransferFunction[fftSize - 1] = imageTransferFunction[0].Conjugate();
						const size_t nyquistFrequency = fftSize / 2;
						for (size_t i = 1; i < nyquistFrequency; i++)
						{
							const heph_float binFrequency = Fourier::IndexToBinFrequency(this->sampleRate, fftSize, i);

							heph_float f_lower = 0;
							heph_float f_upper = 0;
							size_t i_lower = 0;
							size_t i_upper = 0;
							for (size_t j_f = 0; j_f < frequencies2.FrameCount(); j_f++)
							{
								if (frequencies2[j_f] >= binFrequency)
								{
									f_upper = frequencies2[j_f];
									i_upper = j_f;
									break;
								}
								f_lower = frequencies2[j_f];
								i_lower = j_f;
							}
							const heph_float alpha = (binFrequency - f_lower) / (f_upper - f_lower);

							imageTransferFunction[i].real = imagePower[i_lower] * (1.0 - alpha) + imagePower[i_upper] * alpha;
							imageTransferFunction[fftSize - i - 1] = imageTransferFunction[i];
						}

						FloatBuffer imageImpulseRespnose(fftSize);
						Fourier::FFT_Inverse(imageImpulseRespnose, imageTransferFunction);

						const Vector3 image(2 * n * this->roomSize.x - sourceReflections[p].x, 2 * l * this->roomSize.y - sourceReflections[p].y, 2 * m * this->roomSize.z - sourceReflections[p].z);
						const size_t delay = this->sampleRate * image.Distance(reciever) / this->c;

						if (delay + imageImpulseRespnose.FrameCount() >= roomImpulseResponse.FrameCount())
						{
							roomImpulseResponse.Resize(delay + imageImpulseRespnose.FrameCount());
						}

						for (size_t i = 0; i < imageImpulseRespnose.FrameCount(); i++)
						{
							roomImpulseResponse[i + delay] += imageImpulseRespnose[i] * windowBuffer[i];
						}
					}
				}
			}
		}

		return roomImpulseResponse;
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