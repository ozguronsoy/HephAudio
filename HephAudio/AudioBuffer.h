#pragma once
#include "HephAudioFramework.h"
#include "int24.h"
#include "AudioFormatInfo.h"
#include <vector>

namespace HephAudio
{
#pragma region Audio Frame
	class AudioBuffer;
	/// <summary>
	/// Struct for accessing the audio samples at the given channel of the frame.
	/// </summary>
	struct AudioFrame final
	{
		friend class AudioBuffer;
	private:
		const AudioBuffer* pAudioBuffer;
		size_t frameIndex;
		AudioFrame(const AudioBuffer* pAudioBuffer, size_t frameIndex);
	public:
		/// <summary>
		/// Returns the audio sample at the given channel.
		/// </summary>
		/// <param name="channel">Channel of the audio sample.</param>
		/// <returns>The Audio sample.</returns>
		heph_float& operator[](const size_t& channel) const;
	};
#pragma endregion
#pragma region Audio Buffer
	/// <summary>
	/// Holds audio data.
	/// </summary>
	class AudioBuffer final
	{
		friend class AudioProcessor;
	private:
		AudioFormatInfo formatInfo;
		size_t frameCount;
		void* pAudioData;
	public:
		/// <summary>
		/// Initializes an empty buffer.
		/// </summary>
		AudioBuffer();
		/// <summary>
		/// Initializes a buffer with "frameCount" amount of audio frames.
		/// </summary>
		/// <param name="frameCount">The number of frames the buffer will consist of.</param>
		AudioBuffer(size_t frameCount, AudioFormatInfo formatInfo);
		/// <summary>
		/// Initializes a buffer and copies the rhs buffer's content to the new buffer.
		/// </summary>
		AudioBuffer(const AudioBuffer& rhs);
		/// <summary>
		/// Initializes a buffer and moves the rhs buffer's contents to the new buffer.
		/// </summary>
		AudioBuffer(AudioBuffer&& rhs) noexcept;
		/// <summary>
		/// Frees the resources.
		/// </summary>
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
		/// Copies the rhs buffer's content to the current buffer.
		/// </summary>
		AudioBuffer& operator=(const AudioBuffer& rhs);
		/// <summary>
		/// Moves the rhs buffer's content to the current buffer.
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
		AudioBuffer operator*(const heph_float& rhs) const;
		/// <summary>
		/// Multiplies all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		AudioBuffer& operator*=(const heph_float& rhs);
		/// <summary>
		/// Divides all the samples in the current buffer by rhs, then returns the result as a new audio buffer.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		/// <returns>Resulting buffer</returns>
		AudioBuffer operator/(const heph_float& rhs) const;
		/// <summary>
		/// Divides all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		AudioBuffer& operator/=(const heph_float& rhs);
		/// <summary>
		/// Creates a copy of the buffer and shifts it to the left.
		/// </summary>
		/// <param name="rhs">The number of frames to shift</param>
		/// <returns>The shifted buffer.</returns>
		AudioBuffer operator<<(const size_t& rhs) const;
		/// <summary>
		/// Shifts the buffer to the left.
		/// </summary>
		/// <param name="rhs">The number of frames to shift</param>
		AudioBuffer& operator<<=(const size_t& rhs);
		/// <summary>
		/// Creates a copy of the buffer and shifts it to the right.
		/// </summary>
		/// <param name="rhs">The number of frames to shift</param>
		/// <returns>The shifted buffer.</returns>
		AudioBuffer operator>>(const size_t& rhs) const;
		/// <summary>
		/// Shifts the buffer to the right.
		/// </summary>
		/// <param name="rhs">The number of frames to shift</param>
		AudioBuffer& operator>>=(const size_t& rhs);
		/// <summary>
		/// Checks whether the contents of rhs are equal to the current buffer's contents or not.
		/// </summary>
		/// <param name="rhs"></param>
		/// <param name="rhs">The buffer to compare</param>
		/// <returns>true if the contents are equal</returns>
		bool operator==(const AudioBuffer& rhs) const;
		/// <summary>
		/// Checks whether the contents of rhs are equal to the current buffer's contents or not.
		/// </summary>
		/// <param name="rhs">The buffer to compare</param>
		/// <returns>true if the contents are not equal</returns>
		bool operator!=(const AudioBuffer& rhs) const;
		/// <summary>
		/// Calculates the audio data size in bytes.
		/// </summary>
		/// <returns>The audio data size in bytes</returns>
		size_t Size() const noexcept;
		/// <summary>
		/// Returns the number of frames the buffer consists of.
		/// </summary>
		/// <returns>Number of frames the buffer consists of</returns>
		size_t FrameCount() const noexcept;
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
		/// Empties the buffer.
		/// </summary>
		void Empty() noexcept;
		/// <summary>
		/// Finds the sample with the smallest value in the buffer.
		/// </summary>
		/// <returns>The sample with the smallest value.</returns>
		heph_float Min() const noexcept;
		/// <summary>
		/// Finds the sample with the biggest value in the buffer.
		/// </summary>
		/// <returns>The sample with the biggest value.</returns>
		heph_float Max() const noexcept;
		/// <summary>
		/// Finds the sample with the biggest absolute value in the buffer.
		/// </summary>
		/// <returns>The sample with the absolute biggest value.</returns>
		heph_float AbsMax() const noexcept;
		/// <summary>
		/// Calculates the root mean square of the buffer.
		/// </summary>
		/// <returns>the rms of the buffer.</returns>
		heph_float Rms() const noexcept;
		/// <summary>
		/// Calculates how long the audio data is in seconds.
		/// </summary>
		/// <returns>The length of the audio data in seconds</returns>
		heph_float CalculateDuration() const noexcept;
		/// <summary>
		/// Calculates the corresponding frame with the given time.
		/// </summary>
		/// <param name="ts">Time in seconds</param>
		/// <returns>The frame that corresponds to the given time</returns>
		size_t CalculateFrameIndex(heph_float t_s) const noexcept;
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
		/// Changes the number of channels the buffer has.
		/// </summary>
		/// <param name="newChannelCount">New channel count</param>
		void SetChannelCount(uint16_t newChannelCount);
		/// <summary>
		/// Changes the sample rate of the buffer.
		/// </summary>
		/// <param name="newSampleRate">New sample rate</param>
		void SetSampleRate(uint32_t newSampleRate);
		/// <summary>
		/// Gets the start of the audio data.
		/// </summary>
		/// <returns>The start of the audio data</returns>
		void* Begin() const noexcept;
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
		static heph_float CalculateDuration(size_t frameCount, AudioFormatInfo formatInfo) noexcept;
		/// <summary>
		/// Calculates the corresponding frame with the given time.
		/// </summary>
		/// <param name="ts">Time in seconds</param>
		/// <param name="formatInfo">The audio format which will be used in calculation</param>
		/// <returns>The frame that corresponds to the given time</returns>
		static size_t CalculateFrameIndex(heph_float t_s, AudioFormatInfo formatInfo) noexcept;
	};
#pragma endregion
}
HephAudio::AudioBuffer abs(const HephAudio::AudioBuffer& rhs);