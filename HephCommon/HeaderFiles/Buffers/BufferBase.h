#pragma once
#include "HephAudioShared.h"
#include "HephException.h"
#include <cstdlib>

namespace HephCommon
{
	template <typename Tdata>
	class BufferBase
	{
	protected:
		Tdata* pData;

	protected:
		BufferBase() : pData(nullptr) {}
		BufferBase(BufferBase&& rhs) noexcept : pData(rhs.pData)
		{
			rhs.pData = nullptr;
		}

	public:
		virtual ~BufferBase()
		{
			this->Release();
		}

		virtual size_t Size() const = 0;
		virtual size_t SizeAsByte() const = 0;

		virtual bool IsEmpty() const
		{
			return this->pData == nullptr || this->Size() == 0;
		}

		void Reset()
		{
			if (!this->IsEmpty())
			{
				(void)std::memset(this->pData, 0, this->SizeAsByte());
			}
		}

		virtual void Release()
		{
			if (this->pData != nullptr)
			{
				std::free(this->pData);
				this->pData = nullptr;
			}
		}

		Tdata* Begin() const
		{
			return this->pData;
		}

		Tdata* End() const
		{
			return this->pData != nullptr
				? ((Tdata*)(((uint8_t*)this->pData) + this->SizeAsByte()))
				: nullptr;
		}

	protected:
		static Tdata* Allocate(size_t size_byte)
		{
			Tdata* pData = BufferBase<Tdata>::AllocateUninitialized(size_byte);
			(void)std::memset(pData, 0, size_byte);
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

				Tdata* pSubBufferData = BufferBase<Tdata>::AllocateUninitialized(subBufferSize_byte);
				(void)std::memcpy(pSubBufferData, ((uint8_t*)pThisData) + index_byte, copySize_byte);

				// initialize the remaining of the sub buffer
				if (subBufferSize_byte > copySize_byte)
				{
					(void)std::memset(((uint8_t*)pSubBufferData) + copySize_byte, 0, subBufferSize_byte - copySize_byte);
				}

				return pSubBufferData;
			}
			return nullptr;
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
					return BufferBase<Tdata>::Append(pThisData, thisSize_byte, pRhsData, rhsSize_byte);
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

				Tdata* pResultData = BufferBase<Tdata>::AllocateUninitialized(thisSize_byte - cutSize_byte);
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