#pragma once
#include "HephAudioShared.h"
#include "HephException.h"
#include <cstdlib>
#include <type_traits>

namespace HephCommon
{
	enum BufferFlags
	{
		None = 0,
		// do not initialize the elements after allocating memory.
		// used in constructors.
		AllocUninitialized = 1
	};

	template <class Tself, typename Tdata>
	class BufferBase
	{
		static_assert(std::is_default_constructible<Tdata>::value, "Tdata must have a default constructor");

	protected:
		Tdata* pData;
		size_t size;

	protected:
		BufferBase() : pData(nullptr), size(0) {}

		explicit BufferBase(size_t size) : pData(nullptr), size(size)
		{
			if (this->size > 0)
			{
				this->pData = BufferBase::Allocate(this->SizeAsByte());
			}
		}

		BufferBase(size_t size, BufferFlags flags) : pData(nullptr), size(size)
		{
			if (this->size > 0)
			{
				if (flags & BufferFlags::AllocUninitialized)
					this->pData = BufferBase::AllocateUninitialized(this->SizeAsByte());
				else
					this->pData = BufferBase::Allocate(this->SizeAsByte());
			}
		}

		BufferBase(const std::initializer_list<Tdata>& rhs) : pData(nullptr), size(rhs.size())
		{
			if (this->size > 0)
			{
				const size_t size_byte = this->SizeAsByte();
				this->pData = BufferBase::AllocateUninitialized(size_byte);
				(void)std::memcpy(this->pData, rhs.begin(), size_byte);
			}
		}

		BufferBase(const BufferBase& rhs) : pData(nullptr), size(rhs.size)
		{
			if (!rhs.IsEmpty())
			{
				const size_t size_byte = rhs.SizeAsByte();
				this->pData = BufferBase::AllocateUninitialized(size_byte);
				(void)std::memcpy(this->pData, rhs.pData, size_byte);
			}
		}

		BufferBase(BufferBase&& rhs) noexcept : pData(rhs.pData), size(rhs.size)
		{
			rhs.pData = nullptr;
			rhs.size = 0;
		}

	public:
		virtual ~BufferBase()
		{
			this->Release();
		}

		virtual Tself operator<<(size_t rhs) const
		{
			const size_t thisSize_byte = this->SizeAsByte();
			const size_t rhsSize_byte = BufferBase::SizeAsByte(rhs);

			Tself result{};
			dynamic_cast<BufferBase*>(&result)->pData = BufferBase::AllocateUninitialized(thisSize_byte);
			dynamic_cast<BufferBase*>(&result)->size = this->size;

			(void)std::memcpy(dynamic_cast<BufferBase*>(&result)->pData, this->pData + rhs, thisSize_byte - rhsSize_byte);
			if (rhs > 0)
			{
				BufferBase::Initialize(dynamic_cast<BufferBase*>(&result)->pData + this->size - rhs, dynamic_cast<BufferBase*>(&result)->pData + this->size);
			}

			return result;
		}

		virtual BufferBase& operator<<=(size_t rhs)
		{
			if (rhs > 0)
			{
				(void)std::memcpy(this->pData, this->pData + rhs, this->SizeAsByte() - BufferBase::SizeAsByte(rhs));
				BufferBase::Initialize(this->pData + this->size - rhs, this->pData + this->size);
			}
			return *this;
		}

		virtual Tself operator>>(size_t rhs) const
		{
			const size_t thisSize_byte = this->SizeAsByte();

			Tself result{};
			dynamic_cast<BufferBase*>(&result)->pData = BufferBase::AllocateUninitialized(thisSize_byte);
			dynamic_cast<BufferBase*>(&result)->size = this->size;

			(void)std::memcpy(dynamic_cast<BufferBase*>(&result)->pData + rhs, this->pData, thisSize_byte - BufferBase::SizeAsByte(rhs));
			if (rhs > 0)
			{
				BufferBase::Initialize(dynamic_cast<BufferBase*>(&result)->pData, dynamic_cast<BufferBase*>(&result)->pData + rhs);
			}

			return result;
		}

		virtual BufferBase& operator>>=(size_t rhs)
		{
			if (rhs > 0)
			{
				(void)std::memcpy(this->pData + rhs, this->pData, this->SizeAsByte() - BufferBase::SizeAsByte(rhs));
				BufferBase::Initialize(this->pData, this->pData + rhs);
			}
			return *this;
		}

		virtual bool operator==(const Tself& rhs) const
		{
			return (this->IsEmpty() && dynamic_cast<const BufferBase*>(&rhs)->IsEmpty()) ||
				(this->size == dynamic_cast<const BufferBase*>(&rhs)->size && 
					std::memcmp(this->pData, dynamic_cast<const BufferBase*>(&rhs)->pData, this->SizeAsByte()) == 0);
		}

		virtual bool operator!=(const Tself& rhs) const
		{
			return !((*this) == rhs);
		}

		Tdata& operator[](size_t index) const
		{
			return this->pData[index];
		}

		size_t Size() const
		{
			return this->size;
		}

		size_t SizeAsByte() const
		{
			return this->size * sizeof(Tdata);
		}

		Tdata& At(size_t index) const
		{
			if (index >= this->size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::At", "Index out of bounds"));
			}
			return this->pData[index];
		}

		virtual bool IsEmpty() const
		{
			return this->pData == nullptr || this->size == 0;
		}

		virtual void Reset()
		{
			if (!this->IsEmpty())
			{
				BufferBase::Initialize(this->pData, this->pData + this->size);
			}
		}

		virtual void Release()
		{
			if (this->pData != nullptr)
			{
				std::free(this->pData);
				this->pData = nullptr;
			}
			this->size = 0;
		}

		virtual Tself SubBuffer(size_t index, size_t size) const
		{
			Tself result{};

			dynamic_cast<BufferBase*>(&result)->pData = BufferBase::SubBuffer(this->pData, this->SizeAsByte(), BufferBase::SizeAsByte(index), BufferBase::SizeAsByte(size));
			dynamic_cast<BufferBase*>(&result)->size = size;

			return result;
		}

		virtual void Prepend(const Tself& rhs)
		{
			this->pData = BufferBase::Prepend(this->pData, this->SizeAsByte(), dynamic_cast<const BufferBase*>(&rhs)->pData, 
				dynamic_cast<const BufferBase*>(&rhs)->SizeAsByte());
			this->size += dynamic_cast<const BufferBase*>(&rhs)->size;
		}

		virtual void Append(const Tself& rhs)
		{
			this->pData = BufferBase::Append(this->pData, this->SizeAsByte(), dynamic_cast<const BufferBase*>(&rhs)->pData, 
				dynamic_cast<const BufferBase*>(&rhs)->SizeAsByte());
			this->size += dynamic_cast<const BufferBase*>(&rhs)->size;
		}

		virtual void Insert(const Tself& rhs, size_t index)
		{
			this->pData = BufferBase::Insert(this->pData, this->SizeAsByte(), dynamic_cast<const BufferBase*>(&rhs)->pData, 
				dynamic_cast<const BufferBase*>(&rhs)->SizeAsByte(), BufferBase::SizeAsByte(index));
			this->size += dynamic_cast<const BufferBase*>(&rhs)->size;
		}

		virtual void Cut(size_t index, size_t size)
		{
			size_t cutSize_byte = BufferBase::SizeAsByte(size);
			this->pData = BufferBase::Cut(this->pData, this->SizeAsByte(), BufferBase::SizeAsByte(index), cutSize_byte);
			this->size -= cutSize_byte / sizeof(Tdata);
		}

		virtual void Replace(const Tself& rhs, size_t index)
		{
			this->Replace(rhs, index, dynamic_cast<const BufferBase*>(&rhs)->size);
		}

		virtual void Replace(const Tself& rhs, size_t index, size_t size)
		{
			BufferBase::Replace(this->pData, this->SizeAsByte(), dynamic_cast<const BufferBase*>(&rhs)->pData, 
				dynamic_cast<const BufferBase*>(&rhs)->SizeAsByte(), BufferBase::SizeAsByte(index));
		}

		virtual void Resize(size_t newSize)
		{
			if (this->size != newSize)
			{
				if (newSize == 0)
				{
					this->Release();
				}
				else
				{
					Tdata* pTemp = (Tdata*)std::realloc(this->pData, BufferBase::SizeAsByte(newSize));
					if (pTemp == nullptr)
					{
						RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "BufferBase::Resize", "Insufficient memory"));
					}
					if (newSize > this->size)
					{
						BufferBase::Initialize(pTemp + this->size, pTemp + newSize);
					}

					this->pData = pTemp;
					this->size = newSize;
				}
			}
		}

		virtual void Reverse()
		{
			const size_t halfSize = this->size / 2;
			for (size_t i = 0; i < halfSize; ++i)
			{
				std::swap(this->pData[i], this->pData[this->size - i - 1]);
			}
		}

		Tdata* begin() const
		{
			return this->pData;
		}

		Tdata* end() const
		{
			return this->pData != nullptr
				? (this->pData + this->size)
				: nullptr;
		}

	protected:
		static size_t SizeAsByte(size_t size)
		{
			return size * sizeof(Tdata);
		}

		template<typename U = Tdata>
		static typename std::enable_if<std::is_class<U>::value>::type Initialize(U* pData, U* pDataEnd)
		{
			for (; pData < pDataEnd; ++pData)
			{
				(*pData) = U();
			}
		}

		template<typename U = Tdata>
		static typename std::enable_if<not std::is_class<U>::value>::type Initialize(U* pData, U* pDataEnd)
		{
			(void)std::memset(pData, 0, (pDataEnd - pData) * sizeof(U));
		}

		static Tdata* Allocate(size_t size_byte)
		{
			Tdata* pData = BufferBase::AllocateUninitialized(size_byte);
			BufferBase::Initialize(pData, (Tdata*)(((uint8_t*)pData) + size_byte));
			return pData;
		}

		static Tdata* AllocateUninitialized(size_t size_byte)
		{
			Tdata* pData = (Tdata*)std::malloc(size_byte);
			if (pData == nullptr)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "BufferBase::AllocateUninitialized", "Insufficient memory"));
			}
			return pData;
		}

		static Tdata* SubBuffer(Tdata* pThisData, size_t thisSize_byte, size_t index_byte, size_t subBufferSize_byte)
		{
			if (subBufferSize_byte > 0)
			{
				if (pThisData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::SubBuffer", "pRhsData cannot be nullptr"));
				}
				else if (index_byte >= thisSize_byte)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::SubBuffer", "Index out of bounds"));
				}

				size_t copySize_byte = subBufferSize_byte;
				if ((index_byte + copySize_byte) > thisSize_byte)
				{
					copySize_byte = thisSize_byte - index_byte;
				}

				Tdata* pSubBufferData = BufferBase::AllocateUninitialized(subBufferSize_byte);
				(void)std::memcpy(pSubBufferData, ((uint8_t*)pThisData) + index_byte, copySize_byte);

				if (subBufferSize_byte > copySize_byte)
				{
					BufferBase::Initialize((Tdata*)(((uint8_t*)pSubBufferData) + copySize_byte), (Tdata*)(((uint8_t*)pSubBufferData) + subBufferSize_byte));
				}

				return pSubBufferData;
			}
			return nullptr;
		}

		static Tdata* Prepend(Tdata* pThisData, size_t thisSize_byte, Tdata* pRhsData, size_t rhsSize_byte)
		{
			if (rhsSize_byte > 0)
			{
				if (pRhsData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Prepend", "pRhsData cannot be nullptr"));
				}

				Tdata* pResultData = (Tdata*)std::realloc(pThisData, thisSize_byte + rhsSize_byte);
				if (pResultData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "BufferBase::Prepend", "Insufficient memory"));
				}

				if (pThisData == pRhsData)
				{
					(void)std::memcpy(((uint8_t*)pResultData) + rhsSize_byte, pResultData, thisSize_byte);
					(void)std::memcpy(pResultData, ((uint8_t*)pResultData) + rhsSize_byte, rhsSize_byte);
				}
				else
				{
					(void)std::memcpy(((uint8_t*)pResultData) + rhsSize_byte, pThisData, thisSize_byte);
					(void)std::memcpy(pResultData, pRhsData, rhsSize_byte);
				}

				return pResultData;
			}
			return pThisData;
		}

		static Tdata* Append(Tdata* pThisData, size_t thisSize_byte, Tdata* pRhsData, size_t rhsSize_byte)
		{
			if (rhsSize_byte > 0)
			{
				if (pRhsData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Append", "pRhsData cannot be nullptr"));
				}

				Tdata* pResultData = (Tdata*)std::realloc(pThisData, thisSize_byte + rhsSize_byte);
				if (pResultData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "BufferBase::Append", "Insufficient memory"));
				}

				if (pThisData == pRhsData)
				{
					(void)std::memcpy(((uint8_t*)pResultData) + thisSize_byte, pResultData, rhsSize_byte);
				}
				else
				{
					(void)std::memcpy(((uint8_t*)pResultData) + thisSize_byte, pRhsData, rhsSize_byte);
				}

				return pResultData;
			}
			return pThisData;
		}

		static Tdata* Insert(Tdata* pThisData, size_t thisSize_byte, Tdata* pRhsData, size_t rhsSize_byte, size_t index_byte)
		{
			if (rhsSize_byte > 0)
			{
				if (pRhsData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Insert", "pRhsData cannot be nullptr"));
				}
				else if (index_byte > thisSize_byte)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Insert", "Index out of bounds"));
				}
				else if (index_byte == thisSize_byte)
				{
					return BufferBase::Append(pThisData, thisSize_byte, pRhsData, rhsSize_byte);
				}

				Tdata* pResultData = (Tdata*)std::realloc(pThisData, thisSize_byte + rhsSize_byte);
				if (pResultData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "BufferBase::Insert", "Insufficient memory"));
				}

				if (pThisData == pRhsData)
				{
					(void)std::memcpy(((uint8_t*)pResultData) + index_byte + rhsSize_byte, ((uint8_t*)pResultData) + index_byte, thisSize_byte - index_byte);
					if (rhsSize_byte > index_byte)
					{
						(void)std::memcpy(((uint8_t*)pResultData) + index_byte, pResultData, index_byte);
						(void)std::memcpy(((uint8_t*)pResultData) + 2 * index_byte, ((uint8_t*)pResultData) + index_byte + rhsSize_byte, rhsSize_byte - index_byte);
					}
					else
					{
						(void)std::memcpy(((uint8_t*)pResultData) + index_byte, pResultData, rhsSize_byte);
					}
				}
				else
				{
					(void)std::memcpy(((uint8_t*)pResultData) + index_byte + rhsSize_byte, ((uint8_t*)pResultData) + index_byte, thisSize_byte - index_byte);
					(void)std::memcpy(((uint8_t*)pResultData) + index_byte, pRhsData, rhsSize_byte);
				}

				return pResultData;
			}
			return pThisData;
		}

		static Tdata* Cut(Tdata* pThisData, size_t thisSize_byte, size_t index_byte, size_t& cutSize_byte)
		{
			if (thisSize_byte > 0 && cutSize_byte > 0)
			{
				if (pThisData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Cut", "pThisData cannot be nullptr"));
				}
				else if (index_byte >= thisSize_byte)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Cut", "Index out of bounds"));
				}
				else if (index_byte == 0 && cutSize_byte >= thisSize_byte)
				{
					std::free(pThisData);
					cutSize_byte = thisSize_byte;
					return nullptr;
				}

				if ((index_byte + cutSize_byte) > thisSize_byte)
				{
					cutSize_byte = thisSize_byte - index_byte;
				}

				Tdata* pResultData = BufferBase::AllocateUninitialized(thisSize_byte - cutSize_byte);
				if (index_byte > 0)
				{
					(void)std::memcpy(pResultData, pThisData, index_byte);
				}
				if ((index_byte + cutSize_byte) < thisSize_byte)
				{
					(void)std::memcpy(((uint8_t*)pResultData) + index_byte, ((uint8_t*)pThisData) + index_byte + cutSize_byte, thisSize_byte - index_byte - cutSize_byte);
				}

				std::free(pThisData);
				return pResultData;
			}

			cutSize_byte = 0;
			return pThisData;
		}

		static void Replace(Tdata* pThisData, size_t thisSize_byte, Tdata* pRhsData, size_t rhsSize_byte, size_t index_byte)
		{
			if (rhsSize_byte > 0)
			{
				if (pThisData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Replace", "pThisData cannot be nullptr"));
				}
				else if (pRhsData == nullptr)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Replace", "pRhsData cannot be nullptr"));
				}
				else if (index_byte >= thisSize_byte)
				{
					RAISE_AND_THROW_HEPH_EXCEPTION(nullptr, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::Replace", "Index out of bounds"));
				}

				if ((index_byte + rhsSize_byte) > thisSize_byte)
				{
					rhsSize_byte = thisSize_byte - index_byte;
				}
				(void)std::memcpy(((uint8_t*)pThisData) + index_byte, pRhsData, rhsSize_byte);
			}
		}
	};
}