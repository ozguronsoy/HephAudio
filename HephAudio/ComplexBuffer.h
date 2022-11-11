#pragma once
#include "framework.h"
#include "Complex.h"

namespace HephAudio
{
	class ComplexBuffer final
	{
	private:
		Complex* pComplexData;
		size_t frameCount;
	public:
		ComplexBuffer();
		ComplexBuffer(size_t frameCount);
		ComplexBuffer(const ComplexBuffer& rhs);
		~ComplexBuffer();
		Complex& operator[](size_t index) const;
		ComplexBuffer operator-() const;
		ComplexBuffer& operator=(const ComplexBuffer& rhs);
		ComplexBuffer operator+(const ComplexBuffer& rhs) const;
		ComplexBuffer& operator+=(const ComplexBuffer& rhs);
		ComplexBuffer operator*(const Complex& rhs) const;
		ComplexBuffer& operator*=(const Complex& rhs);
		ComplexBuffer operator/(const Complex& rhs) const;
		ComplexBuffer& operator/=(const Complex& rhs);
		ComplexBuffer operator*(const double& rhs) const;
		ComplexBuffer& operator*=(const double& rhs);
		ComplexBuffer operator/(const double& rhs) const;
		ComplexBuffer& operator/=(const double& rhs);
		bool operator==(ComplexBuffer& rhs) const;
		bool operator!=(ComplexBuffer& rhs) const;
		size_t Size() const noexcept;
		size_t FrameCount() const noexcept;
		ComplexBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
		void Join(const ComplexBuffer& buffer);
		void Insert(const ComplexBuffer& buffer, size_t frameIndex);
		void Cut(size_t frameIndex, size_t frameCount);
		void Replace(const ComplexBuffer& buffer, size_t frameIndex);
		void Replace(const ComplexBuffer& buffer, size_t frameIndex, size_t frameCount);
		void Reset();
		void Resize(size_t newFrameCount);
		Complex* GetComplexDataAddress() const noexcept;
	};
}