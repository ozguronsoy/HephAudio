#pragma once
#include "HephAudioShared.h"
#include <cstdint>

#define UINT24_MAX 16777215
#define INT24_MAX 8388607
#define INT24_MIN -8388608

namespace HephAudio
{
	struct int24 final
	{
	private:
		uint8_t bytes[3];
	public:
		int24();
		int24(const int24& rhs);
		int24(const int32_t& rhs);
		operator int32_t() const;
		int24 operator-();
		int24& operator=(const int24& rhs);
		bool operator==(const int24& rhs) const;
		bool operator!=(const int24& rhs) const;
		bool operator>(const int24& rhs) const;
		bool operator>=(const int24& rhs) const;
		bool operator<(const int24& rhs) const;
		bool operator<=(const int24& rhs) const;
		int24 operator<<(const int24& rhs) const;
		int24& operator<<=(const int24& rhs);
		int24 operator>>(const int24& rhs) const;
		int24& operator>>=(const int24& rhs);
		int24 operator^(const int24& rhs) const;
		int24& operator^=(const int24& rhs);
		int24 operator|(const int24& rhs) const;
		int24& operator|=(const int24& rhs);
		int24 operator&(const int24& rhs) const;
		int24& operator&=(const int24& rhs);
		int24 operator+(const int24& rhs) const;
		int24& operator+=(const int24& rhs);
		int24 operator-(const int24& rhs) const;
		int24& operator-=(const int24& rhs);
		int24 operator*(const int24& rhs) const;
		int24& operator*=(const int24& rhs);
		int24 operator/(const int24& rhs) const;
		int24& operator/=(const int24& rhs);
		int24& operator=(const int32_t& rhs);
		bool operator==(const int32_t& rhs) const;
		bool operator!=(const int32_t& rhs) const;
		bool operator>(const int32_t& rhs) const;
		bool operator>=(const int32_t& rhs) const;
		bool operator<(const int32_t& rhs) const;
		bool operator<=(const int32_t& rhs) const;
		int24 operator<<(const int32_t& rhs) const;
		int24& operator<<=(const int32_t& rhs);
		int24 operator>>(const int32_t& rhs) const;
		int24& operator>>=(const int32_t& rhs);
		int24 operator^(const int32_t& rhs) const;
		int24& operator^=(const int32_t& rhs);
		int24 operator|(const int32_t& rhs) const;
		int24& operator|=(const int32_t& rhs);
		int24 operator&(const int32_t& rhs) const;
		int24& operator&=(const int32_t& rhs);
		int24 operator+(const int32_t& rhs) const;
		int24& operator+=(const int32_t& rhs);
		int24 operator-(const int32_t& rhs) const;
		int24& operator-=(const int32_t& rhs);
		int24 operator*(const int32_t& rhs) const;
		int24& operator*=(const int32_t& rhs);
		int24 operator/(const int32_t& rhs) const;
		int24& operator/=(const int32_t& rhs);
	};
}