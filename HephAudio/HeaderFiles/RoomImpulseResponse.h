#pragma once
#include "HephAudioShared.h"
#include "Windows/Window.h"
#include "../HephCommon/HeaderFiles/Vector.h"
#include "../HephCommon/HeaderFiles/FloatBuffer.h"
#include "../HephCommon/HeaderFiles/ComplexBuffer.h"

namespace HephAudio
{
	class RoomImpulseResponse final
	{
	private:
		static constexpr uint32_t p_max = 8;
		static constexpr heph_float q[p_max] = { 0, 0, 0, 0, 1, 1, 1, 1 };
		static constexpr heph_float j[p_max] = { 0, 0, 1, 1, 0, 0, 1, 1 };
		static constexpr heph_float k[p_max] = { 0, 1, 0, 1, 0, 1, 0, 1 };
	public:
		static constexpr uint32_t defaultImageRangeLimit = 10;
	private:
		struct NLM
		{
			int32_t n;
			int32_t l;
			int32_t m;
		};
	private:
		uint32_t sampleRate;
		HephCommon::Vector3 roomSize;
		heph_float wallXY;
		heph_float wallYZ;
		heph_float wallXZ;
		heph_float c; // speed of sound
		heph_float Vroom;
		HephCommon::FloatBuffer frequencies;
		HephCommon::FloatBuffer RT60;
		HephCommon::FloatBuffer BX1; // wall reflection coefficients
		HephCommon::FloatBuffer BX2;
		HephCommon::FloatBuffer BY1;
		HephCommon::FloatBuffer BY2;
		HephCommon::FloatBuffer BZ1;
		HephCommon::FloatBuffer BZ2;
		heph_float impulseResponseRange;
	public:
		RoomImpulseResponse(uint32_t sampleRate, HephCommon::Vector3 roomSize, heph_float c, heph_float frequencyAbsorptionCoefficients[][7], size_t nFrequency);
		HephCommon::FloatBuffer SimulateRoomIR(const HephCommon::Vector3& source, const HephCommon::Vector3& reciever, size_t fftSize, Window& window, uint32_t imageRangeLimit = defaultImageRangeLimit) const;
		HephCommon::ComplexBuffer SimulateRoomTF(const HephCommon::Vector3& source, const HephCommon::Vector3& reciever, size_t fftSize, Window& window, uint32_t imageRangeLimit = defaultImageRangeLimit) const;
		HephCommon::FloatBuffer GetFrequencies() const;
		HephCommon::FloatBuffer GetRT60() const;
		void GetWallReflectionCoefficients(HephCommon::FloatBuffer* pBX1, HephCommon::FloatBuffer* pBX2, HephCommon::FloatBuffer* pBY1, HephCommon::FloatBuffer* pBY2, HephCommon::FloatBuffer* pBZ1, HephCommon::FloatBuffer* pBZ2) const;
		heph_float GetImpulseResponseRange() const;
		uint32_t CalculateNumberOfImages(uint32_t imageRangeLimit = defaultImageRangeLimit) const;
		size_t CalculateImpulseResponseFrameCount(const HephCommon::Vector3& source, const HephCommon::Vector3& reciever, size_t fftSize, uint32_t imageRangeLimit = defaultImageRangeLimit) const;
	private:
		NLM CalculateMaxNLM(uint32_t imageRangeLimit) const;
	};
}