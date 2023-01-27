#pragma once
#include "framework.h"
#include "Complex.h"

namespace HephAudio
{
	class ComplexBuffer final
	{
	private:
		size_t frameCount;
		Complex* pComplexData;
	public:
		ComplexBuffer();
		ComplexBuffer(size_t frameCount);
		ComplexBuffer(const ComplexBuffer& rhs);
		~ComplexBuffer();
		/// <summary>
		/// Returns the complex number at the given index.
		/// </summary>
		/// <param name="index">Position of the complex number.</param>
		/// <returns>The complex number at the given index.</returns>
		Complex& operator[](const size_t& index) const;
		/// <summary>
		/// Creates an inverted complex buffer.
		/// </summary>
		/// <returns>The inverted complex buffer</returns>
		ComplexBuffer operator-() const;
		/// <summary>
		/// Copies the rhs buffer.
		/// </summary>
		ComplexBuffer& operator=(const ComplexBuffer& rhs);
		/// <summary>
		/// Appends the rhs to the current buffer and returns it as a new buffer.
		/// </summary>
		/// <param name="rhs">The buffer which will be joined</param>
		/// <returns>The joint buffer</returns>
		ComplexBuffer operator+(const ComplexBuffer& rhs) const;
		/// <summary>
		/// Appends the rhs to the current buffer.
		/// </summary>
		/// <param name="rhs">The buffer which will be joined</param>
		ComplexBuffer& operator+=(const ComplexBuffer& rhs);
		/// <summary>
		/// Multiplies all the numbers in the current buffer by rhs, then returns the result as a new complex buffer.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		/// <returns>Resulting buffer</returns>
		ComplexBuffer operator*(const Complex& rhs) const;
		/// <summary>
		/// Multiplies all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		ComplexBuffer& operator*=(const Complex& rhs);
		/// <summary>
		/// Divides all the samples in the current buffer by rhs, then returns the result as a new complex buffer.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		/// <returns>Resulting buffer</returns>
		ComplexBuffer operator/(const Complex& rhs) const;
		/// <summary>
		/// Divides all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		ComplexBuffer& operator/=(const Complex& rhs);
		/// <summary>
		/// Multiplies all the numbers in the current buffer by rhs, then returns the result as a new complex buffer.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		/// <returns>Resulting buffer</returns>
		ComplexBuffer operator*(const double& rhs) const;
		/// <summary>
		/// Multiplies all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The multipication factor</param>
		ComplexBuffer& operator*=(const double& rhs);
		/// <summary>
		/// Divides all the samples in the current buffer by rhs, then returns the result as a new complex buffer.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		/// <returns>Resulting buffer</returns>
		ComplexBuffer operator/(const double& rhs) const;
		/// <summary>
		/// Divides all the samples in the current buffer by rhs.
		/// </summary>
		/// <param name="rhs">The division factor</param>
		ComplexBuffer& operator/=(const double& rhs);
		/// <summary>
		/// Checks whether the contents of rhs are equal to the current buffer's contents or not.
		/// </summary>
		/// <param name="rhs"></param>
		/// <param name="rhs">The buffer to compare</param>
		/// <returns>true if the contents are equal</returns>
		bool operator==(const ComplexBuffer& rhs) const;
		/// <summary>
		/// Checks whether the contents of rhs are equal to the current buffer's contents or not.
		/// </summary>
		/// <param name="rhs">The buffer to compare</param>
		/// <returns>true if the contents are not equal</returns>
		bool operator!=(const ComplexBuffer& rhs) const;
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
		/// Gets the desired portion of the complex buffer as a new buffer.
		/// </summary>
		/// <param name="frameIndex">Starting position of the sub buffer</param>
		/// <param name="frameCount">Number of frames the sub buffer will consist of</param>
		/// <returns>A sub buffer</returns>
		ComplexBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		/// <summary>
		/// Appends the given buffer to the current buffer.
		/// </summary>
		/// <param name="buffer">The buffer which will be joined</param>
		void Join(const ComplexBuffer& buffer);
		/// <summary>
		/// Inserts the given buffer to the current buffer.
		/// </summary>
		/// <param name="buffer">The buffer which will be inserted</param>
		/// <param name="frameIndex">Position where the buffer will be inserted</param>
		void Insert(const ComplexBuffer& buffer, size_t frameIndex);
		/// <summary>
		/// Removes the desired portion of the complex buffer.
		/// </summary>
		/// <param name="frameIndex">Starting position of the removed data</param>
		/// <param name="frameCount">Number of frames to remove</param>
		void Cut(size_t frameIndex, size_t frameCount);
		/// <summary>
		/// Replaces the given portion of the complex buffer with the given buffer's data.
		/// </summary>
		/// <param name="buffer">The replacement buffer</param>
		/// <param name="frameIndex">Starting position of the replaced data</param>
		void Replace(const ComplexBuffer& buffer, size_t frameIndex);
		/// <summary>
		/// Replaces the given portion of the complex buffer with the given buffer's data.
		/// </summary>
		/// <param name="buffer">The replacement buffer</param>
		/// <param name="frameIndex">Starting position of the replaced data</param>
		/// <param name="frameCount">Number of frames to replace</param>
		void Replace(const ComplexBuffer& buffer, size_t frameIndex, size_t frameCount);
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
		/// Gets the start of the complex data.
		/// </summary>
		/// <returns>The start of the complex data</returns>
		Complex* const& Begin() const noexcept;
		/// <summary>
		/// Gets the end of the complex data.
		/// </summary>
		/// <returns>The end of the complex data</returns>
		Complex* End() const noexcept;
	};
}