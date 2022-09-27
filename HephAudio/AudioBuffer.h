#pragma once
#include "framework.h"
#include "int24.h"
#include "AudioFormatInfo.h"
#include <vector>

using namespace HephAudio::Structs;

#pragma region Exports
#if defined(_WIN32)
extern "C" __declspec(dllexport) void* _stdcall CreateAudioBuffer(size_t frameCount, void* pFormatInfo);
extern "C" __declspec(dllexport) size_t _stdcall AudioBufferGetSize(void* pAudioBuffer);
extern "C" __declspec(dllexport) size_t _stdcall AudioBufferGetFrameCount(void* pAudioBuffer);
extern "C" __declspec(dllexport) double _stdcall AudioBufferGetSample(void* pAudioBuffer, size_t frameIndex, uint8_t channel);
extern "C" __declspec(dllexport) void _stdcall AudioBufferSetSample(void* pAudioBuffer, double value, size_t frameIndex, uint8_t channel);
extern "C" __declspec(dllexport) void* _stdcall AudioBufferGetSubBuffer(void* pAudioBuffer, size_t frameIndex, size_t frameCount);
extern "C" __declspec(dllexport) void _stdcall AudioBufferJoin(void* pB1, void* pB2);
extern "C" __declspec(dllexport) void _stdcall AudioBufferInsert(void* pB1, size_t frameIndex, void* pB2);
extern "C" __declspec(dllexport) void _stdcall AudioBufferCut(void* pAudioBuffer, size_t frameIndex, size_t frameCount);
extern "C" __declspec(dllexport) void _stdcall AudioBufferReplace(void* pB1, void* pB2, size_t frameIndex, size_t frameCount);
extern "C" __declspec(dllexport) void _stdcall AudioBufferReset(void* pAudioBuffer);
extern "C" __declspec(dllexport) void _stdcall AudioBufferResize(void* pAudioBuffer, size_t newFrameCount);
extern "C" __declspec(dllexport) double _stdcall AudioBufferCalculateDuration(void* pAudioBuffer);
extern "C" __declspec(dllexport) void* _stdcall AudioBufferGetFormat(void* pAudioBuffer);
extern "C" __declspec(dllexport) void _stdcall AudioBufferSetFormat(void* pAudioBuffer, void* newFormat);
extern "C" __declspec(dllexport) void _stdcall DestroyAudioBuffer(void* pAudioBuffer);
#endif
#pragma endregion

namespace HephAudio
{
	class AudioBuffer final
	{
		friend class AudioProcessor;
#if defined(_WIN32)
		friend void* _stdcall ::AudioBufferGetFormat(void* pAudioBuffer);
#endif
	private:
		size_t frameCount;
		void* pAudioData;
		AudioFormatInfo formatInfo;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo);
		AudioBuffer(const AudioBuffer& rhs);
		AudioBuffer& operator=(const AudioBuffer& rhs);
		// Joins the rhs buffer to the end of the current buffer and returns it as a new audio buffer.
		AudioBuffer operator+(const AudioBuffer& rhs) const;
		// Joins the rhs buffer to the end of the current buffer.
		AudioBuffer& operator+=(const AudioBuffer& rhs);
		// Multiplies all the samples in the current buffer by rhs, then returns the result as a new audio buffer.
		AudioBuffer operator*(const double& rhs) const;
		// Multiplies all the samples in the current buffer by rhs.
		AudioBuffer& operator*=(const double& rhs);
		// Divides all the samples in the current buffer by rhs, then returns the result as a new audio buffer.
		AudioBuffer operator/(const double& rhs) const;
		// Divides all the samples in the current buffer by rhs.
		AudioBuffer& operator/=(const double& rhs);
		~AudioBuffer();
		// Buffer size in byte.
		size_t Size() const noexcept;
		size_t FrameCount() const noexcept;
		int32_t GetAsInt32(size_t frameIndex, uint8_t channel) const;
		// Gets normalized sample from the buffer. (for frameIndex = 0 and channel = 0, get sample from the first frames first channel)
		double Get(size_t frameIndex, uint8_t channel) const;
		// value must be between -1 and 1.
		void Set(double value, size_t frameIndex, uint8_t channel);
		AudioBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		// Joins the given buffer to the end of the current buffer.
		void Join(AudioBuffer buffer);
		void Insert(size_t frameIndex, AudioBuffer buffer);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(AudioBuffer buffer, size_t frameIndex);
		void Replace(AudioBuffer buffer, size_t frameIndex, size_t frameCount);
		// Sets all samples in the buffer to 0.
		void Reset();
		void Resize(size_t newFrameCount);
		// Calculates the duration of the buffer in seconds.
		double CalculateDuration() const noexcept;
		AudioFormatInfo GetFormat() const noexcept;
		void SetFormat(AudioFormatInfo newFormat);
		void* GetAudioDataAddress() const noexcept;
	private:
		double GetMin() const noexcept;
		double GetMax() const noexcept;
	public:
		// Calculates the duration of the buffer in seconds.
		static double CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept;
	};
}