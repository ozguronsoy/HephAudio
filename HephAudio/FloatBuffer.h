#pragma once
#include "framework.h"

namespace HephAudio
{
	class FloatBuffer final
	{
	private:
		size_t frameCount;
		hephaudio_float* pData;
	public:
		FloatBuffer();
		FloatBuffer(size_t frameCount);
		FloatBuffer(const FloatBuffer& rhs);
		FloatBuffer(FloatBuffer&& rhs) noexcept;
		~FloatBuffer();
		/// <summary>
		/// Returns the number at the given index.
		/// </summary>
		/// <param name="index">Position of the number.</param>
		/// <returns>The number at the given index.</returns>
		hephaudio_float& operator[](const size_t& frameIndex) const;
		/// <summary>
		/// Creates an inverted float buffer.
		/// </summary>
		/// <returns>The inverted float buffer</returns>
		FloatBuffer operator-() const;
		/// <summary>
		/// Copies the rhs buffer.
		/// </summary>
		FloatBuffer& operator=(const FloatBuffer& rhs);
		/// <summary>
		/// Moves the rhs buffer.
		/// </summary>
		FloatBuffer& operator=(FloatBuffer&& rhs) noexcept;
		/// <summary>
		/// Multiplies all the numbers in the current buffer by rhs, then returns the result as a new buffer.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		/// <returns>Resulting buffer</returns>
		FloatBuffer operator*(const hephaudio_float& rhs) const;
		/// <summary>
		/// Multiplies all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		FloatBuffer& operator*=(const hephaudio_float& rhs);
		/// <summary>
		/// Divides all the samples in the current buffer by rhs, then returns the result as a new buffer.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		/// <returns>Resulting buffer</returns>
		FloatBuffer operator/(const hephaudio_float& rhs) const;
		/// <summary>
		/// Divides all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		FloatBuffer& operator/=(const hephaudio_float& rhs);
		/// <summary>
		/// Creates a copy of the buffer and shifts it to the left.
		/// </summary>
		/// <param name="rhs">The number of frames to shift</param>
		/// <returns>The shifted buffer.</returns>
		FloatBuffer operator<<(const size_t& rhs) const;
		/// <summary>
		/// Shifts the buffer to the left.
		/// </summary>
		/// <param name="rhs">The number of frames to shift</param>
		FloatBuffer& operator<<=(const size_t& rhs);
		/// <summary>
		/// Creates a copy of the buffer and shifts it to the right.
		/// </summary>
		/// <param name="rhs">The number of frames to shift</param>
		/// <returns>The shifted buffer.</returns>
		FloatBuffer operator>>(const size_t& rhs) const;
		/// <summary>
		/// Shifts the buffer to the right.
		/// </summary>
		/// <param name="rhs">The number of frames to shift</param>
		FloatBuffer& operator>>=(const size_t& rhs);
		/// <summary>
		/// Checks whether the contents of rhs are equal to the current buffer's contents or not.
		/// </summary>
		/// <param name="rhs"></param>
		/// <param name="rhs">The buffer to compare</param>
		/// <returns>true if the contents are equal</returns>
		bool operator==(const FloatBuffer& rhs) const;
		/// <summary>
		/// Checks whether the contents of rhs are equal to the current buffer's contents or not.
		/// </summary>
		/// <param name="rhs">The buffer to compare</param>
		/// <returns>true if the contents are not equal</returns>
		bool operator!=(const FloatBuffer& rhs) const;
		/// <summary>
		/// Calculates the buffer size in bytes.
		/// </summary>
		/// <returns>The buffer size in bytes</returns>
		size_t Size() const noexcept;
		/// <summary>
		/// Returns the number of frames the buffer consists of.
		/// </summary>
		/// <returns>Number of frames the buffer consists of</returns>
		size_t FrameCount() const noexcept;
		/// <summary>
		/// Gets the desired portion of the buffer as a new buffer.
		/// </summary>
		/// <param name="frameIndex">Starting position of the sub buffer</param>
		/// <param name="frameCount">Number of frames the sub buffer will consist of</param>
		/// <returns>A sub buffer</returns>
		FloatBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		/// <summary>
		/// Appends the given buffer to the current buffer.
		/// </summary>
		/// <param name="buffer">The buffer which will be joined</param>
		void Join(const FloatBuffer& rhs);
		/// <summary>
		/// Inserts the given buffer to the current buffer.
		/// </summary>
		/// <param name="buffer">The buffer which will be inserted</param>
		/// <param name="frameIndex">Position where the buffer will be inserted</param>
		void Insert(const FloatBuffer& rhs, size_t frameIndex);
		/// <summary>
		/// Removes the desired portion of the buffer.
		/// </summary>
		/// <param name="frameIndex">Starting position of the removed data</param>
		/// <param name="frameCount">Number of frames to remove</param>
		void Cut(size_t frameIndex, size_t frameCount);
		/// <summary>
		/// Replaces the given portion of the buffer with the given buffer's data.
		/// </summary>
		/// <param name="buffer">The replacement buffer</param>
		/// <param name="frameIndex">Starting position of the replaced data</param>
		void Replace(const FloatBuffer& rhs, size_t frameIndex);
		/// <summary>
		/// Replaces the given portion of the buffer with the given buffer's data.
		/// </summary>
		/// <param name="buffer">The replacement buffer</param>
		/// <param name="frameIndex">Starting position of the replaced data</param>
		/// <param name="frameCount">Number of frames to replace</param>
		void Replace(const FloatBuffer& rhs, size_t frameIndex, size_t frameCount);
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
		/// Finds the sample with the smallest value in the buffer.
		/// </summary>
		/// <returns>The sample with the smallest value.</returns>
		hephaudio_float Min() const noexcept;
		/// <summary>
		/// Finds the sample with the biggest value in the buffer.
		/// </summary>
		/// <returns>The sample with the biggest value.</returns>
		hephaudio_float Max() const noexcept;
		/// <summary>
		/// Finds the sample with the biggest absolute value in the buffer.
		/// </summary>
		/// <returns>The sample with the absolute biggest value.</returns>
		hephaudio_float AbsMax() const noexcept;
		/// <summary>
		/// Calculates the root mean square of the buffer.
		/// </summary>
		/// <returns>the rms of the buffer.</returns>
		hephaudio_float Rms() const noexcept;
		/// <summary>
		/// Gets the start of the float data.
		/// </summary>
		/// <returns>The start of the float data</returns>
		hephaudio_float* Begin() const noexcept;
		/// <summary>
		/// Gets the end of the float data.
		/// </summary>
		/// <returns>The end of the float data</returns>
		hephaudio_float* End() const noexcept;
	};
}