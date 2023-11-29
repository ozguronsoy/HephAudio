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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::FloatBuffer", "Insufficient memory."));
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::FloatBuffer", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.begin(), this->Size());
		}
	}
	FloatBuffer::FloatBuffer(const FloatBuffer& rhs) : frameCount(rhs.frameCount), pData(nullptr)
	{
		if (this->frameCount > 0)
		{
			this->pData = (heph_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::FloatBuffer", "Insufficient memory."));
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
	heph_float& FloatBuffer::operator[](size_t frameIndex) const noexcept
	{
		return *(this->pData + frameIndex);
	}
	FloatBuffer FloatBuffer::operator-() const noexcept
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::operator=", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.begin(), this->Size());
		}

		return *this;
	}
	FloatBuffer& FloatBuffer::operator=(const FloatBuffer& rhs)
	{
		this->Empty();

		this->frameCount = rhs.frameCount;
		if (this->frameCount > 0)
		{
			this->pData = (heph_float*)malloc(this->Size());
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::operator=", "Insufficient memory."));
			}
			memcpy(this->pData, rhs.pData, this->Size());
		}

		return *this;
	}
	FloatBuffer& FloatBuffer::operator=(FloatBuffer&& rhs) noexcept
	{
		this->Empty();

		this->frameCount = rhs.frameCount;
		this->pData = rhs.pData;

		rhs.frameCount = 0;
		rhs.pData = nullptr;

		return *this;
	}
	FloatBuffer FloatBuffer::operator+(heph_float rhs) const noexcept
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
	FloatBuffer FloatBuffer::operator-(heph_float rhs) const noexcept
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
	FloatBuffer FloatBuffer::operator*(heph_float rhs) const noexcept
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
	FloatBuffer& FloatBuffer::operator*=(heph_float rhs) noexcept
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
	FloatBuffer FloatBuffer::operator/(heph_float rhs) const noexcept
	{
		FloatBuffer resultBuffer(*this);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] /= rhs;
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator/=(heph_float rhs) noexcept
	{
		for (size_t i = 0; i < this->frameCount; i++)
		{
			(*this)[i] /= rhs;
		}
		return *this;
	}
	FloatBuffer FloatBuffer::operator<<(size_t rhs) const noexcept
	{
		FloatBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData, this->pData + rhs, (this->frameCount - rhs) * sizeof(heph_float));
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator<<=(size_t rhs) noexcept
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
	FloatBuffer FloatBuffer::operator>>(size_t rhs) const noexcept
	{
		FloatBuffer resultBuffer(this->frameCount);
		if (this->frameCount > rhs)
		{
			memcpy(resultBuffer.pData + rhs, this->pData, (this->frameCount - rhs) * sizeof(heph_float));
		}
		return resultBuffer;
	}
	FloatBuffer& FloatBuffer::operator>>=(size_t rhs) noexcept
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
	bool FloatBuffer::operator==(std::nullptr_t rhs) const noexcept
	{
		return this->pData == rhs;
	}
	bool FloatBuffer::operator==(const FloatBuffer& rhs) const noexcept
	{
		return this->pData == rhs.pData || (this->frameCount == rhs.frameCount && this->pData != nullptr && rhs.pData != nullptr && memcmp(this->pData, rhs.pData, this->Size()) == 0);
	}
	bool FloatBuffer::operator!=(std::nullptr_t rhs) const noexcept
	{
		return this->pData != rhs;
	}
	bool FloatBuffer::operator!=(const FloatBuffer& rhs) const noexcept
	{
		return this->pData != rhs.pData && (this->frameCount != rhs.frameCount || this->pData == nullptr || rhs.pData == nullptr || memcmp(this->pData, rhs.pData, this->Size()) != 0);
	}
	size_t FloatBuffer::Size() const noexcept
	{
		return this->frameCount * sizeof(heph_float);
	}
	size_t FloatBuffer::FrameCount() const noexcept
	{
		return this->frameCount;
	}
	heph_float& FloatBuffer::At(size_t frameIndex) const
	{
		if (this->pData == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "FloatBuffer::At", "Empty buffer."));
		}
		if (frameIndex >= this->frameCount)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "FloatBuffer::At", "Index out of bounds."));
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

			// allocate memory with the combined size and copy the rhs's data to the end of the current buffer's data.
			heph_float* pTemp = (heph_float*)malloc(this->Size() + rhs.Size());
			if (pTemp == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::Append", "Insufficient memory."));
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::Insert", "Insufficient memory."));
			}
			memset(pTemp, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to insert start index.
			const size_t frameIndex_byte = frameIndex * sizeof(heph_float);
			if (frameIndex_byte > 0 && oldSize > 0)
			{
				memcpy(pTemp, this->pData, oldSize > frameIndex_byte ? frameIndex_byte : oldSize);
			}

			memcpy(pTemp + frameIndex, rhs.pData, rhs.Size()); // insert the buffer.

			// copy the remaining data.
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

			if (frameIndex + frameCount > this->frameCount) // to prevent overcutting.
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::Cut", "Insufficient memory."));
			}

			const size_t frameIndex_byte = frameIndex * sizeof(heph_float);
			if (frameIndex_byte > 0) // copy from 0 to cut start index.
			{
				memcpy(pTemp, this->pData, frameIndex_byte);
			}

			if (newSize > frameIndex_byte) // copy the remaining data that we didn't cut.
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::Replace", "Insufficient memory."));
			}
			memset(pTemp, 0, newSize); // make sure the padded data is set to 0.

			// copy from 0 to replace start index.
			const size_t frameIndex_byte = frameIndex * sizeof(heph_float);
			if (frameIndex > 0)
			{
				memcpy(pTemp, this->pData, frameIndex_byte > this->Size() ? this->Size() : frameIndex_byte);
			}

			// copy the replace data.
			const size_t replacedSize = frameIndex_byte + frameCount * sizeof(heph_float) >= newSize ? newSize - frameIndex_byte : frameCount * sizeof(heph_float);
			if (replacedSize > 0)
			{
				memcpy(pTemp + frameIndex, rhs.pData, replacedSize);
			}

			// copy the remaining data.
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
	void FloatBuffer::Reset() noexcept
	{
		if (this->pData == nullptr)
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "FloatBuffer::Reset", "Empty buffer."));
			return;
		}

		memset(this->pData, 0, this->Size());
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
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "FloatBuffer::Resize", "Insufficient memory."));
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
	void FloatBuffer::Empty() noexcept
	{
		this->frameCount = 0;
		if (this->pData != nullptr)
		{
			free(this->pData);
			this->pData = nullptr;
		}
	}
	heph_float FloatBuffer::Min() const noexcept
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
	heph_float FloatBuffer::Max() const noexcept
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
	heph_float FloatBuffer::AbsMax() const noexcept
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
	heph_float FloatBuffer::Rms() const noexcept
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
	heph_float* FloatBuffer::Begin() const noexcept
	{
		return this->pData;
	}
	heph_float* FloatBuffer::End() const noexcept
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