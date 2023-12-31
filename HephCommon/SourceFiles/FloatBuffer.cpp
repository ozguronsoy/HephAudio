#include "FloatBuffer.h"
#include "HephException.h"
#include "HephMath.h"

namespace HephCommon
{
	FloatBuffer::FloatBuffer() : frameCount(0), pData(nullptr) {}
	FloatBuffer::FloatBuffer(size_t frameCount) : frameCount(frameCount), pData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pData = (heph_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::FloatBuffer", "Insufficient memory."));
			}
			this->Reset();
		}
	}
	FloatBuffer::FloatBuffer(const std::initializer_list<heph_float>& rhs) : frameCount(rhs.size()), pData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pData = (heph_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::FloatBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.begin(), this->Size());
		}
	}
	FloatBuffer::FloatBuffer(std::nullptr_t rhs) : FloatBuffer() {}
	FloatBuffer::FloatBuffer(const FloatBuffer& rhs) : frameCount(rhs.frameCount), pData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pData = (heph_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::FloatBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.pData, this->Size());
		}
	}
	FloatBuffer::FloatBuffer(FloatBuffer&& rhs) noexcept : frameCount(rhs.frameCount), pData(rhs.pData)
	{
		rhs.frameCount = 0;
		rhs.pData = nullptr;
	}
	FloatBuffer::~FloatBuffer()
	{
		this->Empty();
	}
	heph_float& FloatBuffer::operator[](size_t frameIndex) const
	{
		return *(this->pData + frameIndex);
	}
	FloatBuffer FloatBuffer::operator-() const
	{
		FloatBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = -(*this)[i];
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator=(const std::initializer_list<heph_float>& rhs)
	{
		this->Empty();

		this->frameCount = rhs.size();
		if (this->frameCount > 0)
		{
			this->pData = (heph_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::operator=", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.begin(), this->Size());
		}

		return *this;
	}
	FloatBuffer& FloatBuffer::operator=(std::nullptr_t rhs) 
	{
		this->Empty();
		return *this;
	}
	FloatBuffer& FloatBuffer::operator=(const FloatBuffer& rhs)
	{
		if (this->pData != rhs.pData)
		{
			this->Empty();

			this->frameCount = rhs.frameCount;
			if (this->frameCount > 0)
			{
				this->pData = (heph_float*)malloc(this->Size());
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::operator=", "Insufficient memory."));
				}
				memcpy(this->pData, rhs.pData, this->Size());
			}
		}

		return *this;
	}
	FloatBuffer& FloatBuffer::operator=(FloatBuffer&& rhs) noexcept
	{
		if (this->pData != rhs.pData)
		{
			this->Empty();

			this->frameCount = rhs.frameCount;
			this->pData = rhs.pData;

			rhs.frameCount = 0;
			rhs.pData = nullptr;
		}
		return *this;
	}
	FloatBuffer FloatBuffer::operator+(heph_float rhs) const
	{
		FloatBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] += rhs;
		}
		return resultBuffer;
	}
	FloatBuffer FloatBuffer::operator+(const FloatBuffer& rhs) const
	{
		FloatBuffer resultBuffer(Math::Max(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.frameCount);
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] + rhs[i];
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i] = rhs[i];
		}

		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator+=(heph_float rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] += rhs;
		}
		return *this;
	}
	FloatBuffer& FloatBuffer::operator+=(const FloatBuffer& rhs)
	{
		this->Resize(Math::Max(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] += rhs[i];
		}
		return *this;
	}
	FloatBuffer FloatBuffer::operator-(heph_float rhs) const
	{
		FloatBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] -= rhs;
		}
		return resultBuffer;
	}
	FloatBuffer FloatBuffer::operator-(const FloatBuffer& rhs) const
	{
		FloatBuffer resultBuffer(Math::Max(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.frameCount);
		size_t i;

		for (i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] - rhs[i];
		}

		for (; i < resultBuffer.frameCount; i++)
		{
			resultBuffer[i] = -rhs[i];
		}

		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator-=(heph_float rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] -= rhs;
		}
		return *this;
	}
	FloatBuffer& FloatBuffer::operator-=(const FloatBuffer& rhs)
	{
		this->Resize(Math::Max(this->frameCount, rhs.frameCount));
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] -= rhs[i];
		}
		return *this;
	}
	FloatBuffer FloatBuffer::operator*(heph_float rhs) const
	{
		FloatBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] *= rhs;
		}
		return resultBuffer;
	}
	FloatBuffer FloatBuffer::operator*(const FloatBuffer& rhs) const
	{
		FloatBuffer resultBuffer(Math::Max(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] * rhs[i];
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator*=(heph_float rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] *= rhs;
		}
		return *this;
	}
	FloatBuffer& FloatBuffer::operator*=(const FloatBuffer& rhs)
	{
		if (this->frameCount >= rhs.frameCount)
		{
			for (size_t i = 0; i < rhs.frameCount; i++)
			{
				(*this)[i] *= rhs[i];
			}
			if (this->frameCount > rhs.frameCount)
			{
				memset(this->pData + rhs.frameCount, 0, (this->frameCount - rhs.frameCount) * sizeof(heph_float));
			}
		}
		else
		{
			this->Resize(rhs.frameCount);
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i] *= rhs[i];
			}
		}

		return *this;
	}
	FloatBuffer FloatBuffer::operator/(heph_float rhs) const
	{
		FloatBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] /= rhs;
		}
		return resultBuffer;
	}
	FloatBuffer FloatBuffer::operator/(const FloatBuffer& rhs) const
	{
		FloatBuffer resultBuffer(Math::Max(this->frameCount, rhs.frameCount));
		const size_t minFrameCount = Math::Min(this->frameCount, rhs.frameCount);
		for (size_t i = 0; i < minFrameCount; i++)
		{
			resultBuffer[i] = (*this)[i] / rhs[i];
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator/=(heph_float rhs)
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	FloatBuffer& FloatBuffer::operator/=(const FloatBuffer& rhs)
	{
		if (this->frameCount >= rhs.frameCount)
		{
			for (size_t i = 0; i < rhs.frameCount; i++)
			{
				(*this)[i] /= rhs[i];
			}
			if (this->frameCount > rhs.frameCount)
			{
				memset(this->pData + rhs.frameCount, 0, (this->frameCount - rhs.frameCount) * sizeof(heph_float));
			}
		}
		else
		{
			this->Resize(rhs.frameCount);
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i] /= rhs[i];
			}
		}

		return *this;
	}
	FloatBuffer FloatBuffer::operator<<(size_t rhs) const
	{
		FloatBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData, this->pData + rhs, (this->frameCount - rhs) * sizeof(heph_float));
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator<<=(size_t rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pData, this->pData + rhs, (this->frameCount - rhs) * sizeof(heph_float));
			memset(this->pData + this->frameCount - rhs, 0, rhs * sizeof(heph_float));
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	FloatBuffer FloatBuffer::operator>>(size_t rhs) const
	{
		FloatBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData + rhs, this->pData, (this->frameCount - rhs) * sizeof(heph_float));
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator>>=(size_t rhs)
	{
		if (this->frameCount > rhs)
		{
			memcpy(this->pData + rhs, this->pData, (this->frameCount - rhs) * sizeof(heph_float));
			memset(this->pData, 0, rhs * sizeof(heph_float));
		}
		else
		{
			this->Reset();
		}
		return *this;
	}
	bool FloatBuffer::operator==(std::nullptr_t rhs) const
	{
		return this->pData == rhs;
	}
	bool FloatBuffer::operator==(const FloatBuffer& rhs) const
	{
		return this->pData == rhs.pData || (this->frameCount == rhs.frameCount && this->pData != nullptr && rhs.pData != nullptr && memcmp(this->pData, rhs.pData, this->Size()) == 0);
	}
	bool FloatBuffer::operator!=(std::nullptr_t rhs) const
	{
		return this->pData != rhs;
	}
	bool FloatBuffer::operator!=(const FloatBuffer& rhs) const
	{
		return this->pData != rhs.pData && (this->frameCount != rhs.frameCount || this->pData == nullptr || rhs.pData == nullptr || memcmp(this->pData, rhs.pData, this->Size()) != 0);
	}
	size_t FloatBuffer::Size() const
	{
		return this->frameCount * sizeof(heph_float);
	}
	size_t FloatBuffer::FrameCount() const
	{
		return this->frameCount;
	}
	heph_float& FloatBuffer::At(size_t frameIndex) const
	{
		if (this->pData == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "FloatBuffer::At", "Empty buffer."));
		}
		if (frameIndex >= this->frameCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "FloatBuffer::At", "Index out of bounds."));
		}
		return *(this->pData + frameIndex);
	}
	FloatBuffer FloatBuffer::GetSubBuffer(size_t frameIndex, size_t frameCount) const
	{
		FloatBuffer subBuffer(frameCount);
		if (frameIndex < this->frameCount && frameCount > 0)
		{
			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			memcpy(subBuffer.pData, this->pData + frameIndex, frameCount * sizeof(heph_float));
		}
		return subBuffer;
	}
	void FloatBuffer::Append(const FloatBuffer& rhs)
	{
		if (rhs.frameCount > 0)
		{
			if (this->frameCount == 0)
			{
				*this = rhs;
				return;
			}

			heph_float* pTemp = (heph_float*)malloc(this->Size() + rhs.Size());
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::Append", "Insufficient memory."));
			}

			memcpy(pTemp, this->pData, this->Size());
			memcpy(pTemp + this->frameCount, rhs.pData, rhs.Size());

			free(this->pData);
			this->pData = pTemp;
			this->frameCount += rhs.frameCount;
		}
	}
	void FloatBuffer::Insert(const FloatBuffer& rhs, size_t frameIndex)
	{
		if (rhs.frameCount > 0)
		{
			const size_t oldSize = this->Size();
			const size_t newFrameCount = frameIndex > this->frameCount ? (rhs.frameCount + frameIndex) : (this->frameCount + rhs.frameCount);
			const size_t newSize = newFrameCount * sizeof(heph_float);

			heph_float* pTemp = (heph_float*)malloc(newSize);
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::Insert", "Insufficient memory."));
			}
			memset(pTemp, 0, newSize);

			const size_t frameIndex_byte = frameIndex * sizeof(heph_float);
			if (frameIndex_byte > 0 && oldSize > 0)
			{
				memcpy(pTemp, this->pData, oldSize > frameIndex_byte ? frameIndex_byte : oldSize);
			}

			memcpy(pTemp + frameIndex, rhs.pData, rhs.Size());

			if (oldSize > frameIndex_byte)
			{
				memcpy(pTemp + frameIndex + rhs.frameCount, this->pData + frameIndex, oldSize - frameIndex_byte);
			}

			free(this->pData);
			this->pData = pTemp;
			this->frameCount = newFrameCount;
		}
	}
	void FloatBuffer::Cut(size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0 && frameIndex < this->frameCount)
		{

			if (frameIndex + frameCount > this->frameCount)
			{
				frameCount = this->frameCount - frameIndex;
			}
			if (frameCount == this->frameCount)
			{
				this->Empty();
				return;
			}

			this->frameCount -= frameCount;
			const size_t newSize = this->Size();

			heph_float* pTemp = (heph_float*)malloc(newSize);
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::Cut", "Insufficient memory."));
			}

			const size_t frameIndex_byte = frameIndex * sizeof(heph_float);
			if (frameIndex_byte > 0)
			{
				memcpy(pTemp, this->pData, frameIndex_byte);
			}

			if (newSize > frameIndex_byte)
			{
				memcpy(pTemp + frameIndex, this->pData + frameIndex + frameCount, newSize - frameIndex_byte);
			}

			free(this->pData);
			this->pData = pTemp;
		}
	}
	void FloatBuffer::Replace(const FloatBuffer& rhs, size_t frameIndex)
	{
		this->Replace(rhs, frameIndex, rhs.frameCount);
	}
	void FloatBuffer::Replace(const FloatBuffer& rhs, size_t frameIndex, size_t frameCount)
	{
		if (frameCount > 0)
		{
			const size_t newFrameCount = Math::Max(frameIndex + frameCount, this->frameCount);
			const size_t newSize = newFrameCount * sizeof(heph_float);

			heph_float* pTemp = (heph_float*)malloc(newSize);
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::Replace", "Insufficient memory."));
			}
			memset(pTemp, 0, newSize);

			const size_t frameIndex_byte = frameIndex * sizeof(heph_float);
			if (frameIndex > 0)
			{
				memcpy(pTemp, this->pData, frameIndex_byte > this->Size() ? this->Size() : frameIndex_byte);
			}

			const size_t replacedSize = frameIndex_byte + frameCount * sizeof(heph_float) >= newSize ? newSize - frameIndex_byte : frameCount * sizeof(heph_float);
			if (replacedSize > 0)
			{
				memcpy(pTemp + frameIndex, rhs.pData, replacedSize);
			}

			if (frameIndex + frameCount < this->frameCount)
			{
				const size_t padding = frameIndex_byte + frameCount * sizeof(heph_float);
				memcpy((uint8_t*)pTemp + frameIndex_byte + replacedSize, (uint8_t*)this->pData + padding, this->Size() - padding);
			}

			free(this->pData);
			this->pData = pTemp;
			this->frameCount = newFrameCount;
		}
	}
	void FloatBuffer::Reset()
	{
		if (this->frameCount > 0)
		{
			memset(this->pData, 0, this->Size());
		}
	}
	void FloatBuffer::Resize(size_t newFrameCount)
	{
		if (newFrameCount != this->frameCount)
		{
			if (newFrameCount == 0)
			{
				this->Empty();
			}
			else
			{
				heph_float* pTemp = (heph_float*)realloc(this->pData, newFrameCount * sizeof(heph_float));
				if (pTemp == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "FloatBuffer::Resize", "Insufficient memory."));
				}
				if (newFrameCount > this->frameCount)
				{
					memset(pTemp + this->frameCount, 0, (newFrameCount - this->frameCount) * sizeof(heph_float));
				}
				this->pData = pTemp;
				this->frameCount = newFrameCount;
			}
		}
	}
	void FloatBuffer::Empty()
	{
		this->frameCount = 0;
		if (this->pData != nullptr)
		{
			free(this->pData);
			this->pData = nullptr;
		}
	}
	heph_float FloatBuffer::Min() const
	{
		heph_float minSample = INT32_MAX;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			const heph_float& currentSample = (*this)[i];
			if (currentSample < minSample)
			{
				minSample = currentSample;
			}
		}
		return minSample;
	}
	heph_float FloatBuffer::Max() const
	{
		heph_float maxSample = INT32_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			const heph_float& currentSample = (*this)[i];
			if (currentSample > maxSample)
			{
				maxSample = currentSample;
			}
		}
		return maxSample;
	}
	heph_float FloatBuffer::AbsMax() const
	{
		heph_float maxSample = INT32_MIN;
		for (size_t i = 0; i < this->frameCount; i++)
		{
			const heph_float& currentSample = abs((*this)[i]);
			if (currentSample > maxSample)
			{
				maxSample = currentSample;
			}
		}
		return maxSample;
	}
	heph_float FloatBuffer::Rms() const
	{
		if (this->frameCount != 0)
		{
			heph_float sumOfSamplesSquared = 0.0;
			for (size_t i = 0; i < this->frameCount; i++)
			{
				sumOfSamplesSquared += (*this)[i] * (*this)[i];
			}
			return sqrt(sumOfSamplesSquared / this->frameCount);
		}
		return 0.0;
	}
	FloatBuffer FloatBuffer::Convolve(const FloatBuffer& h) const
	{
		return this->Convolve(h, ConvolutionMode::Full);
	}
	FloatBuffer FloatBuffer::Convolve(const FloatBuffer& h, ConvolutionMode convolutionMode) const
	{
		if (this->frameCount > 0 && h.frameCount > 0)
		{
			size_t yFrameCount = 0;
			size_t iStart = 0;
			size_t iEnd = 0;
			switch (convolutionMode)
			{
			case ConvolutionMode::Central:
				iStart = h.frameCount / 2;
				yFrameCount = this->frameCount;
				iEnd = yFrameCount + iStart;
				break;
			case ConvolutionMode::ValidPadding:
				iStart = h.frameCount - 1;
				yFrameCount = h.frameCount > 0 ? Math::Max(this->frameCount - h.frameCount + 1, (size_t)0) : this->frameCount;
				iEnd = yFrameCount + iStart;
				break;
			case ConvolutionMode::Full:
			default:
				iStart = 0;
				yFrameCount = this->frameCount + h.frameCount - 1;
				iEnd = yFrameCount;
				break;
			}

			FloatBuffer y(yFrameCount);
			for (size_t i = iStart; i < iEnd; i++)
			{
				for (int j = (i < iEnd ? i : (iEnd - 1)); j >= 0 && (i - j) < h.frameCount; j--)
				{
					y.pData[i - iStart] += this->pData[j] * h.pData[i - j];
				}
			}
			return y;
		}
		return FloatBuffer();
	}
	heph_float* FloatBuffer::Begin() const
	{
		return this->pData;
	}
	heph_float* FloatBuffer::End() const
	{
		return this->pData + this->frameCount;
	}
}
HephCommon::FloatBuffer abs(const HephCommon::FloatBuffer& rhs)
{
	HephCommon::FloatBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = abs(rhs[i]);
	}
	return resultBuffer;
}
HephCommon::FloatBuffer operator+(heph_float lhs, const HephCommon::FloatBuffer& rhs)
{
	return rhs + lhs;
}
HephCommon::FloatBuffer operator-(heph_float lhs, const HephCommon::FloatBuffer& rhs)
{
	HephCommon::FloatBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs - rhs[i];
	}
	return resultBuffer;
}
HephCommon::FloatBuffer operator*(heph_float lhs, const HephCommon::FloatBuffer& rhs)
{
	return rhs * lhs;
}
HephCommon::FloatBuffer operator/(heph_float lhs, const HephCommon::FloatBuffer& rhs)
{
	HephCommon::FloatBuffer resultBuffer(rhs.FrameCount());
	for (size_t i = 0; i < resultBuffer.FrameCount(); i++)
	{
		resultBuffer[i] = lhs / rhs[i];
	}
	return resultBuffer;
}