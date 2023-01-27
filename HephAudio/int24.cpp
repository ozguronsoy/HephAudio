#include "int24.h"
#include <memory>

namespace HephAudio
{
	int24::int24()
	{
		memset(this->bytes, 0, sizeof(int24));
	}
	int24::int24(const int24& rhs)
	{
		memcpy(this->bytes, rhs.bytes, sizeof(int24));
	}
	int24::int24(const int32_t& rhs)
	{
		this->bytes[0] = rhs & 0x000000FF;
		this->bytes[1] = (rhs & 0x0000FF00) >> 8;
		this->bytes[2] = (rhs & 0x00FF0000) >> 16;
	}
	int24::operator int32_t() const
	{
		return this->bytes[2] & 0x80 ? ((0xFF000000) | (this->bytes[2] << 16) | (this->bytes[1] << 8) | (this->bytes[0])) : ((this->bytes[2] << 16) | (this->bytes[1] << 8) | (this->bytes[0]));
	}
	int24 int24::operator-()
	{
		return (~(*this) + 1);
	}
	int24& int24::operator=(const int24& rhs)
	{
		memcpy(this->bytes, rhs.bytes, sizeof(int24));
		return *this;
	}
	bool int24::operator==(const int24& rhs) const
	{
		return memcmp(this->bytes, rhs.bytes, sizeof(int24)) == 0;
	}
	bool int24::operator!=(const int24& rhs) const
	{
		return memcmp(this->bytes, rhs.bytes, sizeof(int24)) != 0;
	}
	bool int24::operator>(const int24& rhs) const
	{
		return (rhs.bytes[2] & 0x80) == 0x80 ? memcmp(this->bytes, rhs.bytes, sizeof(int24)) < 0 : memcmp(this->bytes, rhs.bytes, sizeof(int24)) > 0;
	}
	bool int24::operator>=(const int24& rhs) const
	{
		return *this == rhs || *this > rhs;
	}
	bool int24::operator<(const int24& rhs) const
	{
		return (rhs.bytes[2] & 0x80) == 0x80 ? memcmp(this->bytes, rhs.bytes, sizeof(int24)) > 0 : memcmp(this->bytes, rhs.bytes, sizeof(int24)) < 0;
	}
	bool int24::operator<=(const int24& rhs) const
	{
		return *this == rhs || *this < rhs;
	}
	int24 int24::operator<<(const int24& rhs) const
	{
		const int32_t li32 = (0x0000 | (this->bytes[2] << 16) | (this->bytes[1] << 8) | (this->bytes[0]));
		return li32 << (int32_t)rhs;
	}
	int24& int24::operator<<=(const int24& rhs)
	{
		*this = *this << rhs;
		return *this;
	}
	int24 int24::operator>>(const int24& rhs) const
	{
		const int32_t li32 = (0x0000 | (this->bytes[2] << 16) | (this->bytes[1] << 8) | (this->bytes[0]));
		return li32 >> (int32_t)rhs;
	}
	int24& int24::operator>>=(const int24& rhs)
	{
		*this = *this >> rhs;
		return *this;
	}
	int24 int24::operator^(const int24& rhs) const
	{
		const int32_t li32 = (0x0000 | (this->bytes[2] << 16) | (this->bytes[1] << 8) | (this->bytes[0]));
		return li32 ^ (int32_t)rhs;
	}
	int24& int24::operator^=(const int24& rhs)
	{
		*this = *this ^ rhs;
		return *this;
	}
	int24 int24::operator|(const int24& rhs) const
	{
		int24 result = int24();
		result.bytes[0] = this->bytes[0] | rhs.bytes[0];
		result.bytes[1] = this->bytes[1] | rhs.bytes[1];
		result.bytes[2] = this->bytes[2] | rhs.bytes[2];
		return result;
	}
	int24& int24::operator|=(const int24& rhs)
	{
		this->bytes[0] |= rhs.bytes[0];
		this->bytes[1] |= rhs.bytes[1];
		this->bytes[2] |= rhs.bytes[2];
		return *this;
	}
	int24 int24::operator&(const int24& rhs) const
	{
		int24 result = int24();
		result.bytes[0] = this->bytes[0] & rhs.bytes[0];
		result.bytes[1] = this->bytes[1] & rhs.bytes[1];
		result.bytes[2] = this->bytes[2] & rhs.bytes[2];
		return result;
	}
	int24& int24::operator&=(const int24& rhs)
	{
		this->bytes[0] &= rhs.bytes[0];
		this->bytes[1] &= rhs.bytes[1];
		this->bytes[2] &= rhs.bytes[2];
		return *this;
	}
	int24 int24::operator+(const int24& rhs) const
	{
		return (int32_t)*this + (int32_t)rhs;
	}
	int24& int24::operator+=(const int24& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	int24 int24::operator-(const int24& rhs) const
	{
		return (int32_t)*this - (int32_t)rhs;
	}
	int24& int24::operator-=(const int24& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	int24 int24::operator*(const int24& rhs) const
	{
		return (int32_t)*this * (int32_t)rhs;
	}
	int24& int24::operator*=(const int24& rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	int24 int24::operator/(const int24& rhs) const
	{
		return (int32_t)*this / (int32_t)rhs;
	}
	int24& int24::operator/=(const int24& rhs)
	{
		*this = *this / rhs;
		return *this;
	}
	int24& int24::operator=(const int32_t& rhs)
	{
		*this = int24(rhs);
		return *this;
	}
	bool int24::operator==(const int32_t& rhs) const
	{
		return (int32_t)*this == rhs;
	}
	bool int24::operator!=(const int32_t& rhs) const
	{
		return (int32_t)*this != rhs;
	}
	bool int24::operator>(const int32_t& rhs) const
	{
		return (int32_t)*this > rhs;
	}
	bool int24::operator>=(const int32_t& rhs) const
	{
		return *this == rhs || *this > rhs;
	}
	bool int24::operator<(const int32_t& rhs) const
	{
		return (int32_t)*this < rhs;
	}
	bool int24::operator<=(const int32_t& rhs) const
	{
		return *this == rhs || *this < rhs;
	}
	int24 int24::operator<<(const int32_t& rhs) const
	{
		const int32_t li32 = (0x0000 | (this->bytes[2] << 16) | (this->bytes[1] << 8) | (this->bytes[0]));
		return li32 << rhs;
	}
	int24& int24::operator<<=(const int32_t& rhs)
	{
		*this = *this << rhs;
		return *this;
	}
	int24 int24::operator>>(const int32_t& rhs) const
	{
		const int32_t li32 = (0x0000 | (this->bytes[2] << 16) | (this->bytes[1] << 8) | (this->bytes[0]));
		return li32 >> rhs;
	}
	int24& int24::operator>>=(const int32_t& rhs)
	{
		*this = *this >> rhs;
		return *this;
	}
	int24 int24::operator^(const int32_t& rhs) const
	{
		const int32_t li32 = (0x0000 | (this->bytes[2] << 16) | (this->bytes[1] << 8) | (this->bytes[0]));
		return li32 ^ rhs;
	}
	int24& int24::operator^=(const int32_t& rhs)
	{
		*this = *this ^ rhs;
		return *this;
	}
	int24 int24::operator|(const int32_t& rhs) const
	{
		return (int32_t)*this | rhs;
	}
	int24& int24::operator|=(const int32_t& rhs)
	{
		*this = *this | rhs;
		return *this;
	}
	int24 int24::operator&(const int32_t& rhs) const
	{
		return (int32_t)*this & rhs;
	}
	int24& int24::operator&=(const int32_t& rhs)
	{
		*this = *this & rhs;
		return *this;
	}
	int24 int24::operator+(const int32_t& rhs) const
	{
		return (int32_t)*this + rhs;
	}
	int24& int24::operator+=(const int32_t& rhs)
	{
		*this = *this + rhs;
		return *this;
	}
	int24 int24::operator-(const int32_t& rhs) const
	{
		return (int32_t)*this - rhs;
	}
	int24& int24::operator-=(const int32_t& rhs)
	{
		*this = *this - rhs;
		return *this;
	}
	int24 int24::operator*(const int32_t& rhs) const
	{
		return (int32_t)*this * rhs;
	}
	int24& int24::operator*=(const int32_t& rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	int24 int24::operator/(const int32_t& rhs) const
	{
		return (int32_t)*this / rhs;
	}
	int24& int24::operator/=(const int32_t& rhs)
	{
		*this = *this / rhs;
		return *this;
	}
}