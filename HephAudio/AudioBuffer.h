#pragma once
#include "framework.h"
#include "int24.h"
#include "AudioFormatInfo.h"
#include <vector>

namespace HephAudio
{
#pragma region Audio Frame
	class AudioBuffer;
	struct AudioFrame final
	{
		friend class AudioBuffer;
	private:
		double* pAudioData;
		size_t frameIndex;
		size_t channelCount;
		AudioFrame(void* pAudioData, size_t frameIndex, size_t channelCount);
	public:
		double& operator[](const size_t& channel) const;
	};
#pragma endregion
#pragma region Audio Buffer
	class AudioBuffer final
	{
		friend class AudioProcessor;
	private:
		AudioFormatInfo formatInfo;
		size_t frameCount;
		void* pAudioData;
	public:
		AudioBuffer();
		AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo);
		AudioBuffer(const AudioBuffer& rhs);
		AudioBuffer(AudioBuffer&& rhs) noexcept;
		~AudioBuffer();
		/// <summary>
		/// Returns the audio frame at the given index.
		/// </summary>
		/// <param name="frameIndex">Position of the audio frame.</param>
		/// <returns>The audio frame at the given index.</returns>
		AudioFrame operator[](const size_t& frameIndex) const;
		/// <summary>
		/// Creates an inverted audio buffer.
		/// </summary>
		/// <returns>The inverted audio buffer</returns>
		AudioBuffer operator-() const;
		/// <summary>
		/// Copies the rhs buffer.
		/// </summary>
		AudioBuffer& operator=(const AudioBuffer& rhs);
		/// <summary>
		/// Moves the rhs buffer.
		/// </summary>
		AudioBuffer& operator=(AudioBuffer&& rhs) noexcept;
		/// <summary>
		/// Appends the rhs to the current buffer and returns it as a new buffer.
		/// </summary>
		/// <param name="rhs">The buffer which will be joined</param>
		/// <returns>The joint buffer</returns>
		AudioBuffer operator+(const AudioBuffer& rhs) const;
		/// <summary>
		/// Appends the rhs to the current buffer.
		/// </summary>
		/// <param name="rhs">The buffer which will be joined</param>
		AudioBuffer& operator+=(const AudioBuffer& rhs);
		/// <summary>
		/// Multiplies all the samples in the current buffer by rhs, then returns the result as a new audio buffer.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		/// <returns>Resulting buffer</returns>
		AudioBuffer operator*(const double& rhs) const;
		/// <summary>
		/// Multiplies all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		AudioBuffer& operator*=(const double& rhs);
		/// <summary>
		/// Divides all the samples in the current buffer by rhs, then returns the result as a new audio buffer.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		/// <returns>Resulting buffer</returns>
		AudioBuffer operator/(const double& rhs) const;
		/// <summary>
		/// Divides all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		AudioBuffer& operator/=(const double& rhs);
		/// <summary>
		/// Checks whether the contents of rhs are equal to the current buffer's contents or not.
		/// </summary>
		/// <param name="rhs">The buffer to compare</param>
		/// <returns>true if the contents are not equal</returns>
		bool operator!=(const AudioBuffer& rhs) const;
		/// <summary>
		/// Checks whether the contents of rhs are equal to the current buffer's contents or not.
		/// </summary>
		/// <param name="rhs"></param>
		/// <param name="rhs">The buffer to compare</param>
		/// <returns>true if the contents are equal</returns>
		bool operator==(const AudioBuffer& rhs) const;
		/// <summary>
		/// Calculates the audio data size in bytes.
		/// </summary>
		/// <returns>The audio data size in bytes</returns>
		size_t Size() const noexcept;
		/// <summary>
		/// Returns the number of frames the buffer consists of.
		/// </summary>
		/// <returns>Number of frames the buffer consists of</returns>
		const size_t& FrameCount() const noexcept;
		/// <summary>
		/// Gets normalized sample from the buffer.
		/// </summary>
		/// <param name="frameIndex">Position of the frame which contains the sample</param>
		/// <param name="channel">Position of the sample within the frame</param>
		/// <returns>The normalized sample</returns>
		double Get(size_t frameIndex, uint8_t channel) const;
		/// <summary>
		/// Sets the normalized sample at the given position.
		/// </summary>
		/// <param name="value">Normalized audio sample</param>
		/// <param name="frameIndex">Position of the frame which contains the sample</param>
		/// <param name="channel">Position of the sample within the frame</param>
		void Set(double value, size_t frameIndex, uint8_t channel);
		/// <summary>
		/// Gets the desired portion of the audio data, also known as a rectangular window, as a new buffer.
		/// </summary>
		/// <param name="frameIndex">Starting position of the sub buffer</param>
		/// <param name="frameCount">Number of frames the sub buffer will consist of</param>
		/// <returns>A sub buffer</returns>
		AudioBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		/// <summary>
		/// Appends the given buffer to the current buffer.
		/// </summary>
		/// <param name="buffer">The buffer which will be joined</param>
		void Join(const AudioBuffer& buffer);
		/// <summary>
		/// Inserts the given buffer to the current buffer.
		/// </summary>
		/// <param name="buffer">The buffer which will be inserted</param>
		/// <param name="frameIndex">Position where the buffer will be inserted</param>
		void Insert(const AudioBuffer& buffer, size_t frameIndex);
		/// <summary>
		/// Removes the desired portion of the audio data.
		/// </summary>
		/// <param name="frameIndex">Starting position of the removed data</param>
		/// <param name="frameCount">Number of frames to remove</param>
		void Cut(size_t frameIndex, size_t frameCount);
		/// <summary>
		/// Replaces the given portion of the audio data with the given buffer's data.
		/// </summary>
		/// <param name="buffer">The replacement buffer</param>
		/// <param name="frameIndex">Starting position of the replaced data</param>
		void Replace(const AudioBuffer& buffer, size_t frameIndex);
		/// <summary>
		/// Replaces the given portion of the audio data with the given buffer's data.
		/// </summary>
		/// <param name="buffer">The replacement buffer</param>
		/// <param name="frameIndex">Starting position of the replaced data</param>
		/// <param name="frameCount">Number of frames to replace</param>
		void Replace(const AudioBuffer& buffer, size_t frameIndex, size_t frameCount);
		/// <summary>
		/// Sets all samples in the buffer to 0.
		/// </summary>
		void Reset();
		/// <summary>
		/// Resizes the buffer while keeping the data. If the new frame count is less than the current, current buffer will be cut from the end.
		/// </summary>
		/// <param name="newFrameCount">The new number of frames the buffer will consist of</param>
		void Resize(size_t newFrameCount);
		/// <summary>
		/// Calculates how long the audio data is in seconds.
		/// </summary>
		/// <returns>The length of the audio data in seconds</returns>
		double CalculateDuration() const noexcept;
		/// <summary>
		/// Calculates the corresponding frame with the given time.
		/// </summary>
		/// <param name="ts">Time in seconds</param>
		/// <returns>The frame that corresponds to the given time</returns>
		size_t CalculateFrameIndex(double ts) const noexcept;
		/// <summary>
		/// Gets the audio format info.
		/// </summary>
		/// <returns>The audio format info</returns>
		const AudioFormatInfo& FormatInfo() const noexcept;
		/// <summary>
		/// Changes the format of the buffer.
		/// </summary>
		/// <param name="newFormat">New format</param>
		void SetFormat(AudioFormatInfo newFormat);
		/// <summary>
		/// Gets the start of the audio data.
		/// </summary>
		/// <returns>The start of the audio data</returns>
		void* const& Begin() const noexcept;
		/// <summary>
		/// Gets the end of the audio data.
		/// </summary>
		/// <returns>The end of the audio data</returns>
		void* End() const noexcept;
	public:
		/// <summary>
		/// Calculates how long the audio data is in seconds.
		/// </summary>
		/// <param name="frameCount">Number of frames the audio data consists of</param>
		/// <param name="formatInfo">The audio format which will be used in calculation</param>
		/// <returns>The length of the audio data in seconds</returns>
		static double CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept;
		/// <summary>
		/// Calculates the corresponding frame with the given time.
		/// </summary>
		/// <param name="ts">Time in seconds</param>
		/// <param name="formatInfo">The audio format which will be used in calculation</param>
		/// <returns>The frame that corresponds to the given time</returns>
		static size_t CalculateFrameIndex(double ts, AudioFormatInfo formatInfo) noexcept;
	};
#pragma endregion
}