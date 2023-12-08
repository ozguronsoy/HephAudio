#pragma once
#include "HephAudioFramework.h"
#include "Vector.h"
#include "FloatBuffer.h"
#include "ComplexBuffer.h"
#include "Window.h"

namespace HephAudio
{
	class RoomImpulseResponse final
	{
	public:
		static constexpr size_t surfaceCount = 6;
		static constexpr heph_float default_c = 343.0;
		static constexpr uint32_t defaultImageRangeLimit = 10;
	private:
		uint32_t sampleRate;
		HephCommon::Vector3 roomSize;
		heph_float wallXY;
		heph_float wallYZ;
		heph_float wallXZ;
		heph_float c;
		heph_float Vroom;
		HephCommon::FloatBuffer frequencies;
		HephCommon::FloatBuffer RT60;
		HephCommon::FloatBuffer BX1; // reflection coefficients
		HephCommon::FloatBuffer BX2;
		HephCommon::FloatBuffer BY1;
		HephCommon::FloatBuffer BY2;
		HephCommon::FloatBuffer BZ1;
		HephCommon::FloatBuffer BZ2;
		heph_float impulseResponseRange;
	public:
		RoomImpulseResponse(uint32_t sampleRate, HephCommon::Vector3 roomSize, heph_float c, heph_float frequencyAbsorptionCoefficients[][surfaceCount + 1], size_t nFrequency);
		HephCommon::FloatBuffer SimulateRoomIR(HephCommon::Vector3 source, HephCommon::Vector3 reciever, size_t fftSize, Window& window, uint32_t imageRangeLimit = defaultImageRangeLimit) const;
		HephCommon::ComplexBuffer SimulateRoomTF(HephCommon::Vector3 source, HephCommon::Vector3 reciever, size_t fftSize, Window& window, uint32_t imageRangeLimit = defaultImageRangeLimit) const;
		HephCommon::FloatBuffer GetFrequencies() const;
		HephCommon::FloatBuffer GetRT60() const;
		void GetWallReflectionCoefficients(HephCommon::FloatBuffer* pBX1, HephCommon::FloatBuffer* pBX2, HephCommon::FloatBuffer* pBY1, HephCommon::FloatBuffer* pBY2, HephCommon::FloatBuffer* pBZ1, HephCommon::FloatBuffer* pBZ2) const;
		heph_float GetImpulseResponseRange() const;
	};
}