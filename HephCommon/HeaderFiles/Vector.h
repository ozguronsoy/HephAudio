#pragma once
#include "HephCommonFramework.h"
#include "HephException.h"
#include "HephMath.h"

namespace HephCommon
{
	struct Vector3;
	struct Vector2 final
	{
		heph_float x;
		heph_float y;
		constexpr Vector2() : x(0.0), y(0.0) {}
		constexpr Vector2(heph_float x, heph_float y) : x(x), y(y) {}
		constexpr Vector2(const Vector2& rhs) : x(rhs.x), y(rhs.y) {}
		constexpr operator Vector3() const;
		heph_float& operator[](size_t index)
		{
			switch (index)
			{
			case 0:
				return this->x;
			case 1:
				return this->y;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Vector2::operator[]", "Index must be 0 (x) or 1 (y)."));
			}
		}
		constexpr Vector2 operator+() const
		{
			return Vector2(this->x, this->y);
		}
		constexpr Vector2 operator-() const
		{
			return Vector2(-this->x, -this->y);
		}
		constexpr Vector2& operator=(const Vector2& rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;
			return *this;
		}
		constexpr Vector2 operator+(const Vector2& rhs) const
		{
			return Vector2(this->x + rhs.x, this->y + rhs.y);
		}
		constexpr Vector2& operator+=(const Vector2& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			return *this;
		}
		constexpr Vector2 operator-(const Vector2& rhs) const
		{
			return Vector2(this->x - rhs.x, this->y - rhs.y);
		}
		constexpr Vector2& operator-=(const Vector2& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
			return *this;
		}
		constexpr heph_float operator*(const Vector2& rhs) const
		{
			return this->x * rhs.x + this->y * rhs.y;
		}
		constexpr Vector2 operator*(heph_float rhs) const
		{
			return Vector2(this->x * rhs, this->y * rhs);
		}
		constexpr Vector2& operator*=(heph_float rhs)
		{
			this->x *= rhs;
			this->y *= rhs;
			return *this;
		}
		constexpr Vector2 operator/(heph_float rhs) const
		{
			return Vector2(this->x / rhs, this->y / rhs);
		}
		constexpr Vector2& operator/= (heph_float rhs)
		{
			this->x /= rhs;
			this->y /= rhs;
			return *this;
		}
		constexpr bool operator==(const Vector2& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y;
		}
		constexpr bool operator!=(const Vector2& rhs) const
		{
			return this->x != rhs.x || this->y != rhs.y;
		}
		constexpr heph_float MagnitudeSquared() const
		{
			return this->x * this->x + this->y * this->y;
		}
		heph_float Magnitude() const
		{
			return std::sqrt(this->MagnitudeSquared());
		}
		heph_float Angle() const
		{
			return std::atan2(this->y, this->x);
		}
		heph_float Angle(Vector2 rhs) const
		{
			return std::acos((*this) * rhs / (this->Magnitude() * rhs.Magnitude()));
		}
		heph_float Distance(Vector2 rhs) const
		{
			return ((*this) - rhs).Magnitude();
		}
		void Rotate(heph_float angle)
		{
			const heph_float s = std::sin(angle);
			const heph_float c = std::cos(angle);
			const heph_float newX = this->x * c - this->y * s;
			this->y = this->x * s + this->y * c;
			this->x = newX;
		}
	};

	struct Vector3 final
	{
		heph_float x;
		heph_float y;
		heph_float z;
		constexpr Vector3() : x(0.0), y(0.0), z(0.0) {}
		constexpr Vector3(heph_float x, heph_float y) : x(x), y(y), z(0.0) {}
		constexpr Vector3(heph_float x, heph_float y, heph_float z) : x(x), y(y), z(z) {}
		constexpr Vector3(const Vector3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
		constexpr operator Vector2() const;
		heph_float& operator[](size_t index)
		{
			switch (index)
			{
			case 0:
				return this->x;
			case 1:
				return this->y;
			case 2:
				return this->z;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_invalid_argument, "Vector3::operator[]", "Index must be 0 (x), 1 (y) or 2 (z)."));
			}
		}
		constexpr Vector3 operator+() const
		{
			return Vector3(this->x, this->y, this->z);
		}
		constexpr Vector3 operator-() const
		{
			return Vector3(-this->x, -this->y, -this->z);
		}
		constexpr Vector3& operator=(const Vector3& rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;
			return *this;
		}
		constexpr Vector3 operator+(const Vector3& rhs) const
		{
			return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
		}
		constexpr Vector3& operator+=(const Vector3& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			return *this;
		}
		constexpr Vector3 operator-(const Vector3& rhs) const
		{
			return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
		}
		constexpr Vector3& operator-=(const Vector3& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			return *this;
		}
		constexpr heph_float operator*(const Vector3& rhs) const
		{
			return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
		}
		constexpr Vector3 operator^(const Vector3& rhs) const
		{
			return Vector3(
				this->y * rhs.z - this->z * rhs.y,
				this->z * rhs.x - this->x * rhs.z,
				this->x * rhs.y - this->y * rhs.x);
		}
		constexpr Vector3& operator^=(const Vector3& rhs)
		{
			const heph_float newX = this->y * rhs.z - this->z * rhs.y;
			const heph_float newY = this->z * rhs.x - this->x * rhs.z;
			this->z = this->x * rhs.y - this->y * rhs.x;
			this->y = newY;
			this->x = newX;
			return *this;
		}
		constexpr Vector3 operator*(heph_float rhs) const
		{
			return Vector3(this->x * rhs, this->y * rhs, this->z * rhs);
		}
		constexpr Vector3& operator*=(heph_float rhs)
		{
			this->x *= rhs;
			this->y *= rhs;
			this->z *= rhs;
			return *this;
		}
		constexpr Vector3 operator/(heph_float rhs) const
		{
			return Vector3(this->x / rhs, this->y / rhs, this->z / rhs);
		}
		constexpr Vector3& operator/=(heph_float rhs)
		{
			this->x /= rhs;
			this->y /= rhs;
			this->z /= rhs;
			return *this;
		}
		constexpr bool operator==(const Vector3& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
		}
		constexpr bool operator!=(const Vector3& rhs) const
		{
			return this->x != rhs.x || this->y != rhs.y || this->z != rhs.z;
		}
		constexpr heph_float MagnitudeSquared() const
		{
			return this->x * this->x + this->y * this->y + this->z * this->z;
		}
		heph_float Magnitude() const
		{
			return std::sqrt(this->MagnitudeSquared());
		}
		Vector2 Angle() const
		{
			return Vector2(std::atan2(this->y, this->x), std::atan2(this->y, this->z));
		}
		heph_float Angle(Vector2 rhs) const
		{
			return std::acos((*this) * rhs / (this->Magnitude() * rhs.Magnitude()));
		}
		heph_float Distance(Vector3 rhs) const
		{
			return ((*this) - rhs).Magnitude();
		}
		void Rotate(Vector3 angles_rad)
		{
			this->RotateX(angles_rad.x);
			this->RotateY(angles_rad.y);
			this->RotateZ(angles_rad.z);
		}
		void RotateX(heph_float angle)
		{
			const heph_float s = std::sin(angle);
			const heph_float c = std::cos(angle);
			const heph_float newY = this->y * c - this->z * s;
			this->z = this->y * s + this->z * c;
			this->y = newY;
		}
		void RotateY(heph_float angle)
		{
			const heph_float s = std::sin(angle);
			const heph_float c = std::cos(angle);
			const heph_float newX = this->x * c + this->z * s;
			this->z = this->z * c - this->x * s;
			this->x = newX;
		}
		void RotateZ(heph_float angle)
		{
			const heph_float s = std::sin(angle);
			const heph_float c = std::cos(angle);
			const heph_float newX = this->x * c - this->y * s;
			this->y = this->x * s + this->y * c;
			this->x = newX;
		}
	};

	constexpr Vector2::operator Vector3() const
	{
		return Vector3(this->x, this->y);
	}

	constexpr Vector3::operator Vector2() const
	{
		return Vector2(this->x, this->y);
	}
}
inline heph_float abs(const HephCommon::Vector2& rhs)
{
	return rhs.Magnitude();
}
inline heph_float abs(const HephCommon::Vector3& rhs)
{
	return rhs.Magnitude();
}
inline constexpr HephCommon::Vector2 operator*(heph_float lhs, const HephCommon::Vector2& rhs)
{
	return rhs * lhs;
}
inline constexpr HephCommon::Vector3 operator*(heph_float lhs, const HephCommon::Vector3& rhs)
{
	return rhs * lhs;
}