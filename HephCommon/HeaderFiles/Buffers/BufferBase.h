#pragma once
#include "HephAudioShared.h"
#include "HephException.h"
#include <cstdlib>
#include <type_traits>

/** @file */

namespace HephCommon
{
	enum BufferFlags
	{
		None = 0,
		/**
		 * @brief do not initialize the elements after allocating memory.<br>
		 * used in constructors. 
		 * 
		 */
		AllocUninitialized = 1
	};

	/**
	 * @brief base class for buffers. Provides basic buffer operations and methods.
	 * 
	 * @tparam Tself Type of the final buffer that inherits from this class (CRTP).
	 * @tparam Tdata Type of the data the buffer stores.
	 */
	template <class Tself, typename Tdata>
	class BufferBase
	{
		static_assert(std::is_default_constructible<Tdata>::value, "Tdata must have a default constructor");

	protected:
		/**
		 * pointer to the first element of the buffer, or nullptr if the buffer is empty.
		 * 
		 */
		Tdata* pData;

		/**
		 * number of elements the buffer stores.
		 * 
		 */
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

		/**
		 * creates a copy of the current buffer that is shifted to the left.
		 * 
		 * @param rhs number of elements to shift.
		 * 
		 * @throws InsufficientMemoryException
		 */
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

		/**
		 * shifts the current buffer to the left.
		 * 
		 * @param rhs number of elements to shift.
		 */
		virtual BufferBase& operator<<=(size_t rhs)
		{
			if (rhs > 0)
			{
				(void)std::memcpy(this->pData, this->pData + rhs, this->SizeAsByte() - BufferBase::SizeAsByte(rhs));
				BufferBase::Initialize(this->pData + this->size - rhs, this->pData + this->size);
			}
			return *this;
		}

		/**
		 * creates a copy of the current buffer that is shifted to the right.
		 * 
		 * @param rhs number of elements to shift.
		 * 
		 * @throws InsufficientMemoryException
		 */
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

		/**
		 * shifts the current buffer to the right.
		 * 
		 * @param rhs number of elements to shift.
		 */
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

		/**
		 * gets the element at the provided index.
		 * @note this method does not check if the index is valid, use \link HephCommon::BufferBase::At At \endlink method for error checking.
		 * 
		 */
		Tdata& operator[](size_t index) const
		{
			return this->pData[index];
		}

		/**
		 * gets the number of elements stored.
		 * 
		 */
		size_t Size() const
		{
			return this->size;
		}

		/**
		 * gets the size of the buffer in bytes.
		 * 
		 */
		size_t SizeAsByte() const
		{
			return this->size * sizeof(Tdata);
		}

		/**
		 * gets the element at the provided index.
		 * 
		 * @throws InvalidArgumentException
		 */
		Tdata& At(size_t index) const
		{
			if (index >= this->size)
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "BufferBase::At", "Index out of bounds"));
			}
			return this->pData[index];
		}

		/**
		 * checks whether the buffer is empty.
		 * 
		 * @return true if the buffer is empty, otherwise false.
		 */
		virtual bool IsEmpty() const
		{
			return this->pData == nullptr || this->size == 0;
		}

		/**
		 * sets all elements to their default value.
		 * 
		 */
		virtual void Reset()
		{
			if (!this->IsEmpty())
			{
				BufferBase::Initialize(this->pData, this->pData + this->size);
			}
		}

		/**
		 * releases the resources.
		 * 
		 */
		virtual void Release()
		{
			if (this->pData != nullptr)
			{
				std::free(this->pData);
				this->pData = nullptr;
			}
			this->size = 0;
		}

		/**
		 * gets the desired part of the buffer as a new instance.
		 * 
		 * @param index index of the first element of the sub buffer.
		 * @param size number of elements the sub buffer will store.
		 * 
		 * @throws InvalidArgumentException
		 * @throws InsufficientMemoryException
		 */
		virtual Tself SubBuffer(size_t index, size_t size) const
		{
			Tself result{};

			dynamic_cast<BufferBase*>(&result)->pData = BufferBase::SubBuffer(this->pData, this->SizeAsByte(), BufferBase::SizeAsByte(index), BufferBase::SizeAsByte(size));
			dynamic_cast<BufferBase*>(&result)->size = size;

			return result;
		}

		/**
		 * adds the elements of the \a rhs to the start of the current buffer.
		 * 
		 * @param rhs the buffer whose elements will be added.
		 * 
		 * @throws InvalidArgumentException
		 * @throws InsufficientMemoryException
		 */
		virtual void Prepend(const Tself& rhs)
		{
			this->pData = BufferBase::Prepend(this->pData, this->SizeAsByte(), dynamic_cast<const BufferBase*>(&rhs)->pData, 
				dynamic_cast<const BufferBase*>(&rhs)->SizeAsByte());
			this->size += dynamic_cast<const BufferBase*>(&rhs)->size;
		}

		/**
		 * adds the elements of the \a rhs to the end of the current buffer.
		 * 
		 * @param rhs the buffer whose elements will be added.
		 * 
		 * @throws InvalidArgumentException
		 * @throws InsufficientMemoryException
		 */
		virtual void Append(const Tself& rhs)
		{
			this->pData = BufferBase::Append(this->pData, this->SizeAsByte(), dynamic_cast<const BufferBase*>(&rhs)->pData, 
				dynamic_cast<const BufferBase*>(&rhs)->SizeAsByte());
			this->size += dynamic_cast<const BufferBase*>(&rhs)->size;
		}

		/**
		 * adds the elements of the \a rhs to the provided position of the current buffer.
		 * 
		 * @param rhs the buffer whose elements will be added.
		 * 
		 * @throws InvalidArgumentException
		 * @throws InsufficientMemoryException
		 */
		virtual void Insert(const Tself& rhs, size_t index)
		{
			this->pData = BufferBase::Insert(this->pData, this->SizeAsByte(), dynamic_cast<const BufferBase*>(&rhs)->pData, 
				dynamic_cast<const BufferBase*>(&rhs)->SizeAsByte(), BufferBase::SizeAsByte(index));
			this->size += dynamic_cast<const BufferBase*>(&rhs)->size;
		}

		/**
		 * removes the desired portion of the buffer.
		 * 
		 * @param index index of the first element that will be removed.
		 * @param size number of elements to remove.
		 * 
		 * @throws InvalidArgumentException
		 */
		virtual void Cut(size_t index, size_t size)
		{
			size_t cutSize_byte = BufferBase::SizeAsByte(size);
			this->pData = BufferBase::Cut(this->pData, this->SizeAsByte(), BufferBase::SizeAsByte(index), cutSize_byte);
			this->size -= cutSize_byte / sizeof(Tdata);
		}

		/**
		 * replaces the desired portion of the buffer with the contents of the \a rhs.
		 * 
		 * @param rhs the buffer whose elements will be used to replace the current buffer's elements.
		 * @param index index of the first element that will be replaced.
		 * 
		 * @throws InvalidArgumentException
		 */
		virtual void Replace(const Tself& rhs, size_t index)
		{
			this->Replace(rhs, index, dynamic_cast<const BufferBase*>(&rhs)->size);
		}

		/**
		 * replaces the desired portion of the buffer with the contents of the \a rhs.
		 * 
		 * @param rhs the buffer whose elements will be used to replace the current buffer's elements.
		 * @param index index of the first element that will be replaced.
		 * @param size number of elements to replace.
		 * 
		 * @throws InvalidArgumentException
		 */
		virtual void Replace(const Tself& rhs, size_t index, size_t size)
		{
			BufferBase::Replace(this->pData, this->SizeAsByte(), dynamic_cast<const BufferBase*>(&rhs)->pData, 
				dynamic_cast<const BufferBase*>(&rhs)->SizeAsByte(), BufferBase::SizeAsByte(index));
		}

		/**
		 * changes the size of the buffer.<br>
		 * if new size is less than the old, excess elements from the end will be removed.
		 * 
		 * @throws InsufficientMemoryException
		 */
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

		/**
		 * reverses the buffer (first element to last and vice versa).
		 * 
		 */
		virtual void Reverse()
		{
			const size_t halfSize = this->size / 2;
			for (size_t i = 0; i < halfSize; ++i)
			{
				std::swap(this->pData[i], this->pData[this->size - i - 1]);
			}
		}

		/**
		 * gets the pointer to the first element if buffer is not empty, otherwise nullptr.
		 * 
		 */
		Tdata* begin() const
		{
			return this->pData;
		}

		/**
		 * gets the pointer to the end of the buffer (not the last element!) if buffer is not empty, otherwise nullptr.
		 * 
		 */
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