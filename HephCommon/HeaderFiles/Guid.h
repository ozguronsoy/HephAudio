#pragma once
#include "HephShared.h"
#include <cinttypes>

/** @file */

namespace Heph
{
	/**
	 * @brief struct for storing globally unique identifiers.
	 * 
	 */
	struct HEPH_API Guid
	{
		/** first part of the GUID. */
		uint32_t data1;

		/** second part of the GUID. */
		uint16_t data2;

		/** third part of the GUID. */
		uint16_t data3;

		/** fourth part of the GUID. */
		uint8_t data4[8];

		/** @copydoc default_constructor */
		constexpr Guid() : data1(0), data2(0), data3(0), data4{ 0, 0, 0, 0, 0, 0, 0, 0 } {}
		
		constexpr bool operator==(const Guid& rhs) const
		{
			if (this->data1 == rhs.data1 && this->data2 == rhs.data2 && this->data3 == rhs.data3)
			{
				for (size_t i = 0; i < 8; i++)
				{
					if (this->data4[i] != rhs.data4[i])
					{
						return false;
					}
				}
				return true;
			}
			return false;
		}
		
		constexpr bool operator!=(const Guid& rhs) const
		{
			return !(this->operator==(rhs));
		}
		
		/**
		 * generates a new random GUID.
		 * 
		 */
		static Guid GenerateNew();
	};
}