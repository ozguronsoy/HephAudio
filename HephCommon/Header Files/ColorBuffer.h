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
		ColorType* pColorData;
	protected:
		ColorBufferBase() : frameCount(0), pColorData(nullptr) {}
		ColorBufferBase(size_t frameCount) : frameCount(frameCount), pColorData(nullptr)
		{
			if (this->frameCount > 0)
			{
				this->pColorData = (ColorType*)malloc(this->Size());
				if (this->pColorData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ColorBufferBase::ColorBufferBase", "Insufficient memory."));
				}
				this->Reset();
			}
		}
		ColorBufferBase(const std::initializer_list<ColorType>& rhs) : frameCount(rhs.size()), pColorData(nullptr)
		{
			if (this->frameCount > 0)
			{
				this->pColorData = (ColorType*)malloc(this->Size());
				if (this->pColorData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ColorBufferBase::ColorBufferBase", "Insufficient memory."));
				}
				memcpy(this->pColorData, rhs.begin(), this->Size());
			}
		}
		ColorBufferBase(const ColorBufferBase& rhs) : frameCount(rhs.frameCount), pColorData(nullptr)
		{
			if (this->frameCount > 0)
			{
				this->pColorData = (ColorType*)malloc(this->Size());
				if (this->pColorData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ColorBufferBase::ColorBufferBase", "Insufficient memory."));
				}
				memcpy(this->pColorData, rhs.pColorData, this->Size());
			}
		}
		ColorBufferBase(ColorBufferBase&& rhs) : frameCount(rhs.frameCount), pColorData(rhs.pColorData)
		{
			rhs.frameCount = 0;
			rhs.pColorData = nullptr;
		}
	public:
		virtual ~ColorBufferBase()
		{
			this->Empty();
		}
		ColorType& operator[](size_t frameIndex) const noexcept
		{
			return *(this->pColorData + frameIndex);
		}
		ColorBufferBase& operator=(const std::initializer_list<ColorType>& rhs)
		{
			this->Empty();

			this->frameCount = rhs.size();
			if (this->frameCount > 0)
			{
				this->pColorData = (ColorType*)malloc(this->Size());
				if (this->pColorData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ColorBufferBase::operator=", "Insufficient memory."));
				}
				memcpy(this->pColorData, rhs.begin(), this->Size());
			}

			return *this;
		}
		ColorBufferBase& operator=(const ColorBufferBase& rhs)
		{
			this->Empty();

			this->frameCount = rhs.frameCount;
			if (this->frameCount > 0)
			{
				this->pColorData = (ColorType*)malloc(this->Size());
				if (this->pColorData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ColorBufferBase::operator=", "Insufficient memory."));
				}
				memcpy(this->pColorData, rhs.pColorData, this->Size());
			}

			return *this;
		}
		ColorBufferBase& operator=(ColorBufferBase&& rhs) noexcept
		{
			this->Empty();

			this->frameCount = rhs.frameCount;
			this->pColorData = rhs.pColorData;

			rhs.frameCount = 0;
			rhs.pColorData = nullptr;

			return *this;
		}
		bool operator==(const ColorBufferBase& rhs) const noexcept
		{
			return this->pColorData == rhs.pColorData || (this->frameCount == rhs.frameCount && this->pColorData != nullptr && rhs.pColorData != nullptr && memcmp(this->pColorData, rhs.pColorData, this->Size()) == 0);
		}
		bool operator!=(const ColorBufferBase& rhs) const noexcept
		{
			return this->pColorData != rhs.pColorData && (this->frameCount != rhs.frameCount || this->pColorData == nullptr || rhs.pColorData == nullptr || memcmp(this->pColorData, rhs.pColorData, this->Size()) != 0);
		}
		size_t FrameCount() const noexcept
		{
			return this->frameCount;
		}
		size_t Size() const noexcept
		{
			return this->frameCount * sizeof(ColorType);
		}
		ColorType& At(size_t frameIndex) const
		{
			if (this->pColorData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "ColorBufferBase::At", "Empty buffer."));
			}
			if (frameIndex >= this->frameCount)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "ColorBufferBase::At", "Index out of bounds."));
			}
			return *(this->pColorData + frameIndex);
		}
		void Reset() noexcept
		{
			if (this->pColorData == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "ColorBufferBase::Reset", "Empty buffer."));
				return;
			}

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
					ColorType* pTemp = (ColorType*)realloc(this->pColorData, newFrameCount * sizeof(ColorType));
					if (pTemp == nullptr)
					{
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_insufficient_memory, "ColorBufferBase::Resize", "Insufficient memory."));
					}
					if (newFrameCount > this->frameCount)
					{
						memset(pTemp + this->frameCount, 0, (newFrameCount - this->frameCount) * sizeof(ColorType));
					}
					this->pColorData = pTemp;
					this->frameCount = newFrameCount;
				}
			}
		}
		void Empty() noexcept
		{
			if (this->pColorData != nullptr)
			{
				free(this->pColorData);
				this->pColorData = nullptr;
			}
			this->frameCount = 0;
		}
		ColorType* Begin() const noexcept
		{
			return this->pColorData;
		}
		ColorType* End() const noexcept
		{
			return this->pColorData + this->frameCount;
		}
	};

	class ColorBuffer final : public ColorBufferBase<Color>
	{
	private:
		using ColorType = Color;
	public:
		ColorBuffer() : ColorBufferBase<ColorType>() {}
		ColorBuffer(size_t frameCount) : ColorBufferBase<ColorType>(frameCount) {}
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