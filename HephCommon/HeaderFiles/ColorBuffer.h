#pragma once
#include "Color.h"
#include <initializer_list>

namespace HephCommon
{
	class ColorBuffer;
	class RgbBuffer;
	class HslBuffer;
	class HsvBuffer;
	class CmykBuffer;

	template<class ColorType>
	class ColorBufferBase
	{
	protected:
		size_t frameCount;
		ColorType* pData;
	protected:
		ColorBufferBase() : frameCount(0), pData(nullptr) {}
		ColorBufferBase(size_t frameCount) : frameCount(frameCount), pData(nullptr)
		{
			if (this->frameCount > 0)
			{
				this->pData = (ColorType*)malloc(this->Size());
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ColorBufferBase::ColorBufferBase", "Insufficient memory."));
				}
				this->Reset();
			}
		}
		ColorBufferBase(const std::initializer_list<ColorType>& rhs) : frameCount(rhs.size()), pData(nullptr)
		{
			if (this->frameCount > 0)
			{
				this->pData = (ColorType*)malloc(this->Size());
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ColorBufferBase::ColorBufferBase", "Insufficient memory."));
				}
				memcpy(this->pData, rhs.begin(), this->Size());
			}
		}
		ColorBufferBase(std::nullptr_t rhs) : ColorBufferBase() {}
		ColorBufferBase(const ColorBufferBase& rhs) : frameCount(rhs.frameCount), pData(nullptr)
		{
			if (this->frameCount > 0)
			{
				this->pData = (ColorType*)malloc(this->Size());
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ColorBufferBase::ColorBufferBase", "Insufficient memory."));
				}
				memcpy(this->pData, rhs.pData, this->Size());
			}
		}
		ColorBufferBase(ColorBufferBase&& rhs) : frameCount(rhs.frameCount), pData(rhs.pData)
		{
			rhs.frameCount = 0;
			rhs.pData = nullptr;
		}
	public:
		virtual ~ColorBufferBase()
		{
			this->Empty();
		}
		ColorType& operator[](size_t frameIndex) const
		{
			return *(this->pData + frameIndex);
		}
		ColorBufferBase& operator=(const std::initializer_list<ColorType>& rhs)
		{
			this->Empty();

			this->frameCount = rhs.size();
			if (this->frameCount > 0)
			{
				this->pData = (ColorType*)malloc(this->Size());
				if (this->pData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ColorBufferBase::operator=", "Insufficient memory."));
				}
				memcpy(this->pData, rhs.begin(), this->Size());
			}

			return *this;
		}
		ColorBufferBase& operator=(std::nullptr_t rhs) 
		{
			this->Empty();
			return *this;
		}
		ColorBufferBase& operator=(const ColorBufferBase& rhs)
		{
			if (this->pData != rhs.pData)
			{
				this->Empty();

				this->frameCount = rhs.frameCount;
				if (this->frameCount > 0)
				{
					this->pData = (ColorType*)malloc(this->Size());
					if (this->pData == nullptr)
					{
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ColorBufferBase::operator=", "Insufficient memory."));
					}
					memcpy(this->pData, rhs.pData, this->Size());
				}
			}
			return *this;
		}
		ColorBufferBase& operator=(ColorBufferBase&& rhs) noexcept
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
		bool operator==(const ColorBufferBase& rhs) const
		{
			return this->pData == rhs.pData || (this->frameCount == rhs.frameCount && this->pData != nullptr && rhs.pData != nullptr && memcmp(this->pData, rhs.pData, this->Size()) == 0);
		}
		bool operator==(std::nullptr_t rhs) const
		{
			return this->pData == nullptr;
		}
		bool operator!=(const ColorBufferBase& rhs) const
		{
			return this->pData != rhs.pData && (this->frameCount != rhs.frameCount || this->pData == nullptr || rhs.pData == nullptr || memcmp(this->pData, rhs.pData, this->Size()) != 0);
		}
		bool operator!=(std::nullptr_t rhs) const
		{
			return this->pData != nullptr;
		}
		size_t FrameCount() const
		{
			return this->frameCount;
		}
		size_t Size() const
		{
			return this->frameCount * sizeof(ColorType);
		}
		ColorType& At(size_t frameIndex) const
		{
			if (this->pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "ColorBufferBase::At", "Empty buffer."));
			}
			if (frameIndex >= this->frameCount)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "ColorBufferBase::At", "Index out of bounds."));
			}
			return *(this->pData + frameIndex);
		}
		void Reset()
		{
			for (size_t i = 0; i < this->frameCount; i++)
			{
				(*this)[i] = ColorType();
			}
		}
		void Resize(size_t newFrameCount)
		{
			if (newFrameCount != this->frameCount)
			{
				if (newFrameCount == 0)
				{
					this->Empty();
				}
				else
				{
					ColorType* pTemp = (ColorType*)realloc(this->pData, newFrameCount * sizeof(ColorType));
					if (pTemp == nullptr)
					{
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "ColorBufferBase::Resize", "Insufficient memory."));
					}
					if (newFrameCount > this->frameCount)
					{
						memset(pTemp + this->frameCount, 0, (newFrameCount - this->frameCount) * sizeof(ColorType));
					}
					this->pData = pTemp;
					this->frameCount = newFrameCount;
				}
			}
		}
		void Empty()
		{
			if (this->pData != nullptr)
			{
				free(this->pData);
				this->pData = nullptr;
			}
			this->frameCount = 0;
		}
		ColorType* Begin() const
		{
			return this->pData;
		}
		ColorType* End() const
		{
			return this->pData + this->frameCount;
		}
	};

	class ColorBuffer final : public ColorBufferBase<Color>
	{
	private:
		using ColorType = Color;
	public:
		ColorBuffer() : ColorBufferBase<ColorType>() {}
		ColorBuffer(size_t frameCount) : ColorBufferBase<ColorType>(frameCount) {}
		ColorBuffer(std::nullptr_t rhs) : ColorBufferBase<ColorType>(rhs) {}
		ColorBuffer(const ColorBuffer& rhs) : ColorBufferBase<ColorType>(rhs) {}
		ColorBuffer(ColorBuffer&& rhs) noexcept : ColorBufferBase<ColorType>(rhs) {}
		explicit operator RgbBuffer() const;
		explicit operator HslBuffer() const;
		explicit operator HsvBuffer() const;
		explicit operator CmykBuffer() const;
	};

	class RgbBuffer final : public ColorBufferBase<RGB>
	{
	private:
		using ColorType = RGB;
	public:
		RgbBuffer() : ColorBufferBase<ColorType>() {}
		RgbBuffer(size_t frameCount) : ColorBufferBase<ColorType>(frameCount) {}
		RgbBuffer(std::nullptr_t rhs) : ColorBufferBase<ColorType>(rhs) {}
		RgbBuffer(const RgbBuffer& rhs) : ColorBufferBase<ColorType>(rhs) {}
		RgbBuffer(RgbBuffer&& rhs) noexcept : ColorBufferBase<ColorType>(rhs) {}
		explicit operator ColorBuffer() const;
		explicit operator HslBuffer() const;
		explicit operator HsvBuffer() const;
		explicit operator CmykBuffer() const;
	};

	class HslBuffer final : public ColorBufferBase<HSL>
	{
	private:
		using ColorType = HSL;
	public:
		HslBuffer() : ColorBufferBase<ColorType>() {}
		HslBuffer(size_t frameCount) : ColorBufferBase<ColorType>(frameCount) {}
		HslBuffer(std::nullptr_t rhs) : ColorBufferBase<ColorType>(rhs) {}
		HslBuffer(const HslBuffer& rhs) : ColorBufferBase<ColorType>(rhs) {}
		HslBuffer(HslBuffer&& rhs) noexcept : ColorBufferBase<ColorType>(rhs) {}
		explicit operator ColorBuffer() const;
		explicit operator RgbBuffer() const;
		explicit operator HsvBuffer() const;
		explicit operator CmykBuffer() const;
	};

	class HsvBuffer final : public ColorBufferBase<HSV>
	{
	private:
		using ColorType = HSV;
	public:
		HsvBuffer() : ColorBufferBase<ColorType>() {}
		HsvBuffer(size_t frameCount) : ColorBufferBase<ColorType>(frameCount) {}
		HsvBuffer(const HsvBuffer& rhs) : ColorBufferBase<ColorType>(rhs) {}
		HsvBuffer(std::nullptr_t rhs) : ColorBufferBase<ColorType>(rhs) {}
		HsvBuffer(HsvBuffer&& rhs) noexcept : ColorBufferBase<ColorType>(rhs) {}
		explicit operator ColorBuffer() const;
		explicit operator RgbBuffer() const;
		explicit operator HslBuffer() const;
		explicit operator CmykBuffer() const;
	};

	class CmykBuffer final : public ColorBufferBase<CMYK>
	{
	private:
		using ColorType = CMYK;
	public:
		CmykBuffer() : ColorBufferBase<ColorType>() {}
		CmykBuffer(size_t frameCount) : ColorBufferBase<ColorType>(frameCount) {}
		CmykBuffer(std::nullptr_t rhs) : ColorBufferBase<ColorType>(rhs) {}
		CmykBuffer(const CmykBuffer& rhs) : ColorBufferBase<ColorType>(rhs) {}
		CmykBuffer(CmykBuffer&& rhs) noexcept : ColorBufferBase<ColorType>(rhs) {}
		explicit operator ColorBuffer() const;
		explicit operator RgbBuffer() const;
		explicit operator HslBuffer() const;
		explicit operator HsvBuffer() const;
	};

	ColorBuffer::operator RgbBuffer() const
	{
		RgbBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	ColorBuffer::operator HslBuffer() const
	{
		HslBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	ColorBuffer::operator HsvBuffer() const
	{
		HsvBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	ColorBuffer::operator CmykBuffer() const
	{
		CmykBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}

	RgbBuffer::operator ColorBuffer() const
	{
		ColorBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	RgbBuffer::operator HslBuffer() const
	{
		HslBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	RgbBuffer::operator HsvBuffer() const
	{
		HsvBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	RgbBuffer::operator CmykBuffer() const
	{
		CmykBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}

	HslBuffer::operator ColorBuffer() const
	{
		ColorBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	HslBuffer::operator RgbBuffer() const
	{
		RgbBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	HslBuffer::operator HsvBuffer() const
	{
		HsvBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	HslBuffer::operator CmykBuffer() const
	{
		CmykBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}

	HsvBuffer::operator ColorBuffer() const
	{
		ColorBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	HsvBuffer::operator RgbBuffer() const
	{
		RgbBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	HsvBuffer::operator HslBuffer() const
	{
		HslBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	HsvBuffer::operator CmykBuffer() const
	{
		CmykBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}

	CmykBuffer::operator ColorBuffer() const
	{
		ColorBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	CmykBuffer::operator RgbBuffer() const
	{
		RgbBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	CmykBuffer::operator HslBuffer() const
	{
		HslBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
	CmykBuffer::operator HsvBuffer() const
	{
		HsvBuffer resultBuffer(this->frameCount);
		for (size_t i = 0; i < this->frameCount; i++)
		{
			resultBuffer[i] = (*this)[i];
		}
		return resultBuffer;
	}
}