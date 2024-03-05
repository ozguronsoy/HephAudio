#pragma once
#include "HephCommonShared.h"
#include "HephException.h"
#include "StringBuffer.h"
#include "HephMath.h"
#include <cinttypes>

#if !defined(HEPH_COLOR_CHANNEL_TYPE)

#if defined(HEPH_COLOR_CHANNEL_TYPE_FLOAT)

typedef float heph_color_channel;
#define HEPH_COLOR_CHANNEL_MIN ((heph_color_channel)0.0f)
#define HEPH_COLOR_CHANNEL_MAX ((heph_color_channel)1.0f)
#define HEPH_COLOR_CHANNEL_TYPE_I 0

#elif defined(HEPH_COLOR_CHANNEL_TYPE_UINT32)

typedef uint32_t heph_color_channel;
#define HEPH_COLOR_CHANNEL_MIN ((heph_color_channel)0x00000000u)
#define HEPH_COLOR_CHANNEL_MAX ((heph_color_channel)0xFFFFFFFFu)
#define HEPH_COLOR_CHANNEL_TYPE_I 1

#elif defined(HEPH_COLOR_CHANNEL_TYPE_UINT16)

typedef uint16_t heph_color_channel;
#define HEPH_COLOR_CHANNEL_MIN ((heph_color_channel)0x0000u)
#define HEPH_COLOR_CHANNEL_MAX ((heph_color_channel)0xFFFFu)
#define HEPH_COLOR_CHANNEL_TYPE_I 1

#else

typedef uint8_t heph_color_channel;
#define HEPH_COLOR_CHANNEL_MIN ((heph_color_channel)0x00u)
#define HEPH_COLOR_CHANNEL_MAX ((heph_color_channel)0xFFu)
#define HEPH_COLOR_CHANNEL_TYPE_I 1

#endif

#define HEPH_COLOR_CHANNEL_TYPE heph_color_channel

#endif

namespace HephCommon
{
	struct RGB;
	struct HSL;
	struct HSV;
	struct CMYK;

	struct Color final
	{
		friend struct RGB;
		friend struct HSL;
		friend struct HSV;
		friend struct CMYK;
	public:
		heph_color_channel r;
		heph_color_channel g;
		heph_color_channel b;
		heph_color_channel a;
		constexpr Color() : r(HEPH_COLOR_CHANNEL_MAX), g(HEPH_COLOR_CHANNEL_MAX), b(HEPH_COLOR_CHANNEL_MAX), a(HEPH_COLOR_CHANNEL_MAX) {}
		constexpr Color(heph_color_channel r, heph_color_channel g, heph_color_channel b) : r(r), g(g), b(b), a(HEPH_COLOR_CHANNEL_MAX) {}
		constexpr Color(heph_color_channel r, heph_color_channel g, heph_color_channel b, heph_color_channel a) : r(r), g(g), b(b), a(a) {}
		constexpr Color(const char* hexString) : Color()
		{
			this->FromHexString(hexString);
		}
		constexpr Color(const wchar_t* hexString) : Color()
		{
			this->FromHexString(hexString);
		}
		Color(const StringBuffer& hexString) : Color()
		{
			if (hexString.GetStringType() == StringType::ASCII)
			{
				this->FromHexString(hexString.c_str());
			}
			else
			{
				this->FromHexString(hexString.wc_str());
			}
		}
		constexpr operator RGB() const;
		constexpr operator HSL() const;
		constexpr operator HSV() const;
		constexpr operator CMYK() const;
		heph_color_channel& operator[](size_t index)
		{
			switch (index)
			{
			case 0:
				return this->r;
			case 1:
				return this->g;
			case 2:
				return this->b;
			case 3:
				return this->a;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Color::operator[]", "Index must be 0 (r), 1 (g), 2 (b) or 3 (a)."));
			}
		}
		constexpr Color& operator=(const Color& rhs)
		{
			this->r = rhs.r;
			this->g = rhs.g;
			this->b = rhs.b;
			this->a = rhs.a;
			return *this;
		}
		constexpr Color& operator=(const char* rhs)
		{
			this->FromHexString(rhs);
			return *this;
		}
		constexpr Color& operator=(const wchar_t* rhs)
		{
			this->FromHexString(rhs);
			return *this;
		}
		Color& operator=(const StringBuffer& rhs)
		{
			if (rhs.GetStringType() == StringType::ASCII)
			{
				return this->operator=(rhs.c_str());
			}
			return this->operator=(rhs.wc_str());
		}
		constexpr bool operator==(const Color& rhs) const
		{
			return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b && this->a == rhs.a;
		}
		constexpr bool operator!=(const Color& rhs) const
		{
			return this->r != rhs.r || this->g != rhs.g || this->b != rhs.b || this->a != rhs.a;
		}
		StringBuffer ToHexString() const
		{

			StringBuffer colorString = "#00000000";

			uint8_t channel = this->r / (float)HEPH_COLOR_CHANNEL_MAX * UINT8_MAX;
			colorString[1] = IntToHexChar(channel >> 4);
			colorString[2] = IntToHexChar(channel & 0x0F);

			channel = this->g / (float)HEPH_COLOR_CHANNEL_MAX * UINT8_MAX;
			colorString[3] = IntToHexChar(channel >> 4);
			colorString[4] = IntToHexChar(channel & 0x0F);

			channel = this->b / (float)HEPH_COLOR_CHANNEL_MAX * UINT8_MAX;
			colorString[5] = IntToHexChar(channel >> 4);
			colorString[6] = IntToHexChar(channel & 0x0F);

			channel = this->a / (float)HEPH_COLOR_CHANNEL_MAX * UINT8_MAX;
			colorString[7] = IntToHexChar(channel >> 4);
			colorString[8] = IntToHexChar(channel & 0x0F);
		}
	private:
		constexpr heph_color_channel& GetChannelByIndex(size_t index)
		{
			switch (index)
			{
			case 0:
				return this->r;
			case 1:
				return this->g;
			case 2:
				return this->b;
			case 3:
			default:
				return this->a;
			}
		}
		constexpr void FromHexString(const char* hexString)
		{
			uint32_t stringSize = 0;
			while (hexString[stringSize] != '\0')
			{
				stringSize++;
			}
			uint32_t i2 = 0;

			if (stringSize >= 3)
			{
				if (hexString[0] == '#')
				{
					if (stringSize == 3)
					{
						return;
					}
					i2++;
				}

				if ((stringSize >= 6 && stringSize < 8 && i2 == 0) || (stringSize >= 7 && stringSize < 9 && i2 == 1))
				{
					for (uint32_t i = 0; i < 3; i++, i2 += 2)
					{
						this->GetChannelByIndex(i) = ((HexCharToInt(hexString[i2]) << 4) | HexCharToInt(hexString[i2 + 1])) / (float)UINT8_MAX * HEPH_COLOR_CHANNEL_MAX;
					}
					this->a = HEPH_COLOR_CHANNEL_MAX;
				}
				else if ((stringSize >= 8 && i2 == 0) || (stringSize >= 9 && i2 == 1))
				{
					for (uint32_t i = 0; i < 4; i++, i2 += 2)
					{
						this->GetChannelByIndex(i) = ((HexCharToInt(hexString[i2]) << 4) | HexCharToInt(hexString[i2 + 1])) / (float)UINT8_MAX * HEPH_COLOR_CHANNEL_MAX;
					}
				}
				else
				{
					for (uint32_t i = 0; i < 3; i++, i2++)
					{
						this->GetChannelByIndex(i) = ((HexCharToInt(hexString[i2]) << 4) | HexCharToInt(hexString[i2])) / (float)UINT8_MAX * HEPH_COLOR_CHANNEL_MAX;
					}
				}
			}
		}
		constexpr void FromHexString(const wchar_t* hexString)
		{
			uint32_t stringSize = 0;
			while (hexString[stringSize] != L'\0')
			{
				stringSize++;
			}
			uint32_t i2 = 0;

			if (stringSize >= 3)
			{
				if (hexString[0] == L'#')
				{
					if (stringSize == 3)
					{
						return;
					}
					i2++;
				}

				if ((stringSize >= 6 && stringSize < 8 && i2 == 0) || (stringSize >= 7 && stringSize < 9 && i2 == 1))
				{
					for (uint32_t i = 0; i < 3; i++, i2 += 2)
					{
						this->GetChannelByIndex(i) = ((HexCharToInt(hexString[i2]) << 4) | HexCharToInt(hexString[i2 + 1])) / (float)UINT8_MAX * HEPH_COLOR_CHANNEL_MAX;
					}
					this->a = HEPH_COLOR_CHANNEL_MAX;
				}
				else if ((stringSize >= 8 && i2 == 0) || (stringSize >= 9 && i2 == 1))
				{
					for (uint32_t i = 0; i < 4; i++, i2 += 2)
					{
						this->GetChannelByIndex(i) = ((HexCharToInt(hexString[i2]) << 4) | HexCharToInt(hexString[i2 + 1])) / (float)UINT8_MAX * HEPH_COLOR_CHANNEL_MAX;
					}
				}
				else
				{
					for (uint32_t i = 0; i < 3; i++, i2++)
					{
						this->GetChannelByIndex(i) = ((HexCharToInt(hexString[i2]) << 4) | HexCharToInt(hexString[i2])) / (float)UINT8_MAX * HEPH_COLOR_CHANNEL_MAX;
					}
				}
			}
		}
		static constexpr uint8_t HexCharToInt(char c)
		{
			switch (c)
			{
			case '0':
				return 0x00;
			case '1':
				return 0x01;
			case '2':
				return 0x02;
			case '3':
				return 0x03;
			case '4':
				return 0x04;
			case '5':
				return 0x05;
			case '6':
				return 0x06;
			case '7':
				return 0x07;
			case '8':
				return 0x08;
			case '9':
				return 0x09;
			case 'a':
			case 'A':
				return 0x0A;
			case 'b':
			case 'B':
				return 0x0B;
			case 'c':
			case 'C':
				return 0x0C;
			case 'd':
			case 'D':
				return 0x0D;
			case 'e':
			case 'E':
				return 0x0E;
			case 'f':
			case 'F':
				return 0x0F;
			default:
				return 0x00;
			}
		}
		static constexpr uint8_t HexCharToInt(wchar_t c)
		{
			switch (c)
			{
			case L'0':
				return 0x00;
			case L'1':
				return 0x01;
			case L'2':
				return 0x02;
			case L'3':
				return 0x03;
			case L'4':
				return 0x04;
			case L'5':
				return 0x05;
			case L'6':
				return 0x06;
			case L'7':
				return 0x07;
			case L'8':
				return 0x08;
			case L'9':
				return 0x09;
			case L'a':
			case L'A':
				return 0x0A;
			case L'b':
			case L'B':
				return 0x0B;
			case L'c':
			case L'C':
				return 0x0C;
			case L'd':
			case L'D':
				return 0x0D;
			case L'e':
			case L'E':
				return 0x0E;
			case L'f':
			case L'F':
				return 0x0F;
			default:
				return 0x00;
			}
		}
		static constexpr char IntToHexChar(uint8_t i)
		{
			switch (i)
			{
			case 0x00:
				return '0';
			case 0x01:
				return '1';
			case 0x02:
				return '2';
			case 0x03:
				return '3';
			case 0x04:
				return '4';
			case 0x05:
				return '5';
			case 0x06:
				return '6';
			case 0x07:
				return '7';
			case 0x08:
				return '8';
			case 0x09:
				return '9';
			case 0x0A:
				return 'A';
			case 11:
				return 'B';
			case 0x0C:
				return 'C';
			case 0x0D:
				return 'D';
			case 0x0E:
				return 'E';
			case 0x0F:
				return 'F';
			default:
				return '0';
			}
		}
	};

	struct RGB final
	{
		heph_color_channel r;
		heph_color_channel g;
		heph_color_channel b;
		constexpr RGB() : r(HEPH_COLOR_CHANNEL_MAX), g(HEPH_COLOR_CHANNEL_MAX), b(HEPH_COLOR_CHANNEL_MAX) {}
		constexpr RGB(heph_color_channel r, heph_color_channel g, heph_color_channel b) : r(r), g(g), b(b) {}
		constexpr RGB(const char* hexString) : RGB()
		{
			const Color temp = Color(hexString);
			this->r = temp.r;
			this->g = temp.g;
			this->b = temp.b;
		}
		constexpr RGB(const wchar_t* hexString) : RGB()
		{
			const Color temp = Color(hexString);
			this->r = temp.r;
			this->g = temp.g;
			this->b = temp.b;
		}
		RGB(const StringBuffer& hexString) : RGB()
		{
			const Color temp = Color(hexString);
			this->r = temp.r;
			this->g = temp.g;
			this->b = temp.b;
		}
		constexpr operator Color() const;
		constexpr operator HSL() const;
		constexpr operator HSV() const;
		constexpr operator CMYK() const;
		heph_color_channel& operator[](size_t index)
		{
			switch (index)
			{
			case 0:
				return this->r;
			case 1:
				return this->g;
			case 2:
				return this->b;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "RGB::operator[]", "Index must be 0 (r), 1 (g), or 2 (b)."));
			}
		}
		constexpr RGB& operator=(const RGB& rhs)
		{
			this->r = rhs.r;
			this->g = rhs.g;
			this->b = rhs.b;
			return *this;
		}
		constexpr RGB& operator=(const char* rhs)
		{
			const Color temp = Color(rhs);
			this->r = temp.r;
			this->g = temp.g;
			this->b = temp.b;
			return *this;
		}
		constexpr RGB& operator=(const wchar_t* rhs)
		{
			const Color temp = Color(rhs);
			this->r = temp.r;
			this->g = temp.g;
			this->b = temp.b;
			return *this;
		}
		RGB& operator=(const StringBuffer& rhs)
		{
			if (rhs.GetStringType() == StringType::ASCII)
			{
				return this->operator=(rhs.c_str());
			}
			return this->operator=(rhs.wc_str());
		}
		constexpr bool operator==(const RGB& rhs) const
		{
			return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b;
		}
		constexpr bool operator!=(const RGB& rhs) const
		{
			return this->r != rhs.r || this->g != rhs.g || this->b != rhs.b;
		}
	};

	struct HSL final
	{
		float h;
		float s;
		float l;
		constexpr HSL() : h(0), s(0), l(1.0f) {}
		constexpr HSL(float h, float s, float l) : h(h), s(s), l(l) {}
		constexpr HSL(const char* hexString) : HSL()
		{
			const HSL hsl = Color(hexString);
			this->h = hsl.h;
			this->s = hsl.s;
			this->l = hsl.l;
		}
		constexpr HSL(const wchar_t* hexString) : HSL()
		{
			const HSL temp = Color(hexString);
			this->h = temp.h;
			this->s = temp.s;
			this->l = temp.l;
		}
		HSL(const StringBuffer& hexString) : HSL()
		{
			const HSL temp = Color(hexString);
			this->h = temp.h;
			this->s = temp.s;
			this->l = temp.l;
		}
		constexpr operator Color() const;
		constexpr operator RGB() const;
		constexpr operator HSV() const;
		constexpr operator CMYK() const;
		float& operator[](size_t index)
		{
			switch (index)
			{
			case 0:
				return this->h;
			case 1:
				return this->s;
			case 2:
				return this->l;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "HSL::operator[]", "Index must be 0 (h), 1 (s), or 2 (l)."));
			}
		}
		constexpr HSL& operator=(const HSL& rhs)
		{
			this->h = rhs.h;
			this->s = rhs.s;
			this->l = rhs.l;
			return *this;
		}
		constexpr HSL& operator=(const char* rhs)
		{
			const HSL temp = Color(rhs);
			this->h = temp.h;
			this->s = temp.s;
			this->l = temp.l;
			return *this;
		}
		constexpr HSL& operator=(const wchar_t* rhs)
		{
			const HSL temp = Color(rhs);
			this->h = temp.h;
			this->s = temp.s;
			this->l = temp.l;
			return *this;
		}
		HSL& operator=(const StringBuffer& rhs)
		{
			if (rhs.GetStringType() == StringType::ASCII)
			{
				return this->operator=(rhs.c_str());
			}
			return this->operator=(rhs.wc_str());
		}
		constexpr bool operator==(const HSL& rhs) const
		{
			return this->h == rhs.h && this->s == rhs.s && this->l == rhs.l;
		}
		constexpr bool operator!=(const HSL& rhs) const
		{
			return this->h != rhs.h || this->s != rhs.s || this->l != rhs.l;
		}
	};

	struct HSV final
	{
		float h;
		float s;
		float v;
		constexpr HSV() : h(0), s(0), v(1.0f) {}
		constexpr HSV(float h, float s, float v) : h(h), s(s), v(v) {}
		constexpr HSV(const char* hexString) : HSV()
		{
			const HSV hsl = Color(hexString);
			this->h = hsl.h;
			this->s = hsl.s;
			this->v = hsl.v;
		}
		constexpr HSV(const wchar_t* hexString) : HSV()
		{
			const HSV temp = Color(hexString);
			this->h = temp.h;
			this->s = temp.s;
			this->v = temp.v;
		}
		HSV(const StringBuffer& hexString) : HSV()
		{
			const HSV temp = Color(hexString);
			this->h = temp.h;
			this->s = temp.s;
			this->v = temp.v;
		}
		constexpr operator Color() const;
		constexpr operator RGB() const;
		constexpr operator HSL() const;
		constexpr operator CMYK() const;
		float& operator[](size_t index)
		{
			switch (index)
			{
			case 0:
				return this->h;
			case 1:
				return this->s;
			case 2:
				return this->v;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "HSV::operator[]", "Index must be 0 (h), 1 (s), or 2 (v)."));
			}
		}
		constexpr HSV& operator=(const HSV& rhs)
		{
			this->h = rhs.h;
			this->s = rhs.s;
			this->v = rhs.v;
			return *this;
		}
		constexpr HSV& operator=(const char* rhs)
		{
			const HSV temp = Color(rhs);
			this->h = temp.h;
			this->s = temp.s;
			this->v = temp.v;
			return *this;
		}
		constexpr HSV& operator=(const wchar_t* rhs)
		{
			const HSV temp = Color(rhs);
			this->h = temp.h;
			this->s = temp.s;
			this->v = temp.v;
			return *this;
		}
		HSV& operator=(const StringBuffer& rhs)
		{
			if (rhs.GetStringType() == StringType::ASCII)
			{
				return this->operator=(rhs.c_str());
			}
			return this->operator=(rhs.wc_str());
		}
		constexpr bool operator==(const HSV& rhs) const
		{
			return this->h == rhs.h && this->s == rhs.s && this->v == rhs.v;
		}
		constexpr bool operator!=(const HSV& rhs) const
		{
			return this->h != rhs.h || this->s != rhs.s || this->v != rhs.v;
		}
	};

	struct CMYK final
	{
		float c;
		float m;
		float y;
		float k;
		constexpr CMYK() : c(0), m(0), y(0), k(0) {}
		constexpr CMYK(float c, float m, float y, float k) : c(c), m(m), y(y), k(k) {}
		constexpr CMYK(const char* hexString) : CMYK()
		{
			const CMYK temp = Color(hexString);
			this->c = temp.c;
			this->m = temp.m;
			this->y = temp.y;
			this->k = temp.k;
		}
		constexpr CMYK(const wchar_t* hexString) : CMYK()
		{
			const CMYK temp = Color(hexString);
			this->c = temp.c;
			this->m = temp.m;
			this->y = temp.y;
			this->k = temp.k;
		}
		CMYK(const StringBuffer& hexString) : CMYK()
		{
			const CMYK temp = Color(hexString);
			this->c = temp.c;
			this->m = temp.m;
			this->y = temp.y;
			this->k = temp.k;
		}
		constexpr operator Color() const;
		constexpr operator RGB() const;
		constexpr operator HSL() const;
		constexpr operator HSV() const;
		float& operator[](size_t index)
		{
			switch (index)
			{
			case 0:
				return this->c;
			case 1:
				return this->m;
			case 2:
				return this->y;
			case 3:
				return this->k;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "CMYK::operator[]", "Index must be 0 (c), 1 (m), 2 (y), or 3 (k)."));
			}
		}
		constexpr CMYK& operator=(const CMYK& rhs)
		{
			this->c = rhs.c;
			this->m = rhs.m;
			this->y = rhs.y;
			this->k = rhs.k;
			return *this;
		}
		constexpr CMYK& operator=(const char* rhs)
		{
			const CMYK temp = Color(rhs);
			this->c = temp.c;
			this->m = temp.m;
			this->y = temp.y;
			this->k = temp.k;
			return *this;
		}
		constexpr CMYK& operator=(const wchar_t* rhs)
		{
			const CMYK temp = Color(rhs);
			this->c = temp.c;
			this->m = temp.m;
			this->y = temp.y;
			this->k = temp.k;
			return *this;
		}
		CMYK& operator=(const StringBuffer& rhs)
		{
			if (rhs.GetStringType() == StringType::ASCII)
			{
				return this->operator=(rhs.c_str());
			}
			return this->operator=(rhs.wc_str());
		}
		constexpr bool operator==(const CMYK& rhs) const
		{
			return this->c == rhs.c && this->m == rhs.m && this->y == rhs.y && this->k == rhs.k;
		}
		constexpr bool operator!=(const CMYK& rhs) const
		{
			return this->c != rhs.c || this->m != rhs.m || this->y != rhs.y || this->k != rhs.k;
		}
	};

	constexpr Color::operator RGB() const
	{
		return RGB(this->r, this->g, this->b);
	}
	constexpr Color::operator HSL() const
	{
		HSL hsl;

		const float n_r = (float)this->r / (float)HEPH_COLOR_CHANNEL_MAX;
		const float n_g = (float)this->g / (float)HEPH_COLOR_CHANNEL_MAX;
		const float n_b = (float)this->b / (float)HEPH_COLOR_CHANNEL_MAX;

		const float c_max = Math::Max(Math::Max(n_r, n_g), n_b);
		const float c_min = Math::Min(Math::Min(n_r, n_g), n_b);
		const float delta = c_max - c_min;

		hsl.l = (c_max + c_min) * 0.5f;
		hsl.s = (delta == 0 ? 0 : (delta / (1.0f - Math::Abs(c_max + c_min - 1.0f))));

		if (delta == 0)
		{
			hsl.h = 0;
		}
		else if (c_max == n_r)
		{
			hsl.h = 60.0f * ((n_g - n_b) / delta);
		}
		else if (c_max == n_g)
		{
			hsl.h = 60.0f * (2.0f + (n_b - n_r) / delta);
		}
		else
		{
			hsl.h = 60.0f * (4.0f + (n_r - n_g) / delta);
		}
		while (hsl.h >= 360)
		{
			hsl.h -= 360;
		}
		while (hsl.h < 0)
		{
			hsl.h += 360;
		}

		return hsl;
	}
	constexpr Color::operator HSV() const
	{
		HSV hsv;

		const float n_r = (float)this->r / (float)HEPH_COLOR_CHANNEL_MAX;
		const float n_g = (float)this->g / (float)HEPH_COLOR_CHANNEL_MAX;
		const float n_b = (float)this->b / (float)HEPH_COLOR_CHANNEL_MAX;

		const float c_max = Math::Max(Math::Max(n_r, n_g), n_b);
		const float c_min = Math::Min(Math::Min(n_r, n_g), n_b);
		const float delta = c_max - c_min;

		hsv.v = c_max;
		hsv.s = c_max == 0 ? 0 : (delta / c_max);

		if (delta == 0)
		{
			hsv.h = 0;
		}
		else if (c_max == n_r)
		{
			hsv.h = 60.0f * ((n_g - n_b) / delta);
		}
		else if (c_max == n_g)
		{
			hsv.h = 60.0f * (2.0f + (n_b - n_r) / delta);
		}
		else
		{
			hsv.h = 60.0f * (4.0f + (n_r - n_g) / delta);
		}
		while (hsv.h >= 360)
		{
			hsv.h -= 360;
		}
		while (hsv.h < 0)
		{
			hsv.h += 360;
		}

		return hsv;
	}
	constexpr Color::operator CMYK() const
	{
		const float n_r = (float)this->r / (float)HEPH_COLOR_CHANNEL_MAX;
		const float n_g = (float)this->g / (float)HEPH_COLOR_CHANNEL_MAX;
		const float n_b = (float)this->b / (float)HEPH_COLOR_CHANNEL_MAX;
		const float maxChannel = Math::Max(Math::Max(n_r, n_g), n_b);
		return CMYK((maxChannel - n_r) / maxChannel, (maxChannel - n_g) / maxChannel, (maxChannel - n_b) / maxChannel, 1.0f - maxChannel);
	}

	constexpr RGB::operator Color() const
	{
		return Color(this->r, this->g, this->b);
	}
	constexpr RGB::operator HSL() const
	{
		return (HSL)(Color)(*this);
	}
	constexpr RGB::operator HSV() const
	{
		return (HSV)(Color)(*this);
	}
	constexpr RGB::operator CMYK() const
	{
		return (CMYK)(Color)(*this);
	}

	constexpr HSL::operator Color() const
	{
		const float c = (1.0f - Math::Abs(2.0f * this->l - 1.0f)) * this->s;
		const float x = c * (1.0f - Math::Abs(Math::Mod(this->h / 60.0f, 2.0f) - 1.0f));
		const float m = this->l - c * 0.5f;

		float n_r = m, n_g = m, n_b = m;
		if (this->h < 60)
		{
			n_r += c;
			n_g += x;
		}
		else if (this->h < 120)
		{
			n_r += x;
			n_g += c;
		}
		else if (this->h < 180)
		{
			n_g += c;
			n_b += x;
		}
		else if (this->h < 240)
		{
			n_g += x;
			n_b += c;
		}
		else if (this->h < 300)
		{
			n_r += x;
			n_b += c;
		}
		else
		{
			n_r += c;
			n_b += x;
		}

#if HEPH_COLOR_CHANNEL_TYPE_I == 0
		return Color(n_r * HEPH_COLOR_CHANNEL_MAX, n_g * HEPH_COLOR_CHANNEL_MAX, n_b * HEPH_COLOR_CHANNEL_MAX);
#else
		return Color(Math::Round(n_r * HEPH_COLOR_CHANNEL_MAX), Math::Round(n_g * HEPH_COLOR_CHANNEL_MAX), Math::Round(n_b * HEPH_COLOR_CHANNEL_MAX));
#endif
	}
	constexpr HSL::operator RGB() const
	{
		return (RGB)(Color)(*this);
	}
	constexpr HSL::operator HSV() const
	{
		HSV hsv;
		hsv.h = this->h;
		hsv.v = this->l + this->s * Math::Min(this->l, 1.0f - this->l);
		hsv.s = hsv.v == 0 ? 0 : (2.0f * (1.0f - this->l / hsv.v));
		return hsv;
	}
	constexpr HSL::operator CMYK() const
	{
		return (CMYK)(Color)(*this);
	}

	constexpr HSV::operator Color() const
	{
		const float c = this->v * this->s;
		const float x = c * (1.0f - Math::Abs(Math::Mod(this->h / 60.0f, 2.0f) - 1.0f));
		const float m = this->v - c;

		float n_r = m, n_g = m, n_b = m;
		if (this->h < 60)
		{
			n_r += c;
			n_g += x;
		}
		else if (this->h < 120)
		{
			n_r += x;
			n_g += c;
		}
		else if (this->h < 180)
		{
			n_g += c;
			n_b += x;
		}
		else if (this->h < 240)
		{
			n_g += x;
			n_b += c;
		}
		else if (this->h < 300)
		{
			n_r += x;
			n_b += c;
		}
		else
		{
			n_r += c;
			n_b += x;
		}

#if HEPH_COLOR_CHANNEL_TYPE_I == 0
		return Color(n_r * HEPH_COLOR_CHANNEL_MAX, n_g * HEPH_COLOR_CHANNEL_MAX, n_b * HEPH_COLOR_CHANNEL_MAX);
#else
		return Color(Math::Round(n_r * HEPH_COLOR_CHANNEL_MAX), Math::Round(n_g * HEPH_COLOR_CHANNEL_MAX), Math::Round(n_b * HEPH_COLOR_CHANNEL_MAX));
#endif
	}
	constexpr HSV::operator RGB() const
	{
		return (RGB)(Color)(*this);
	}
	constexpr HSV::operator HSL() const
	{
		HSL hsl;
		hsl.h = this->h;
		hsl.l = this->v * (1.0f - this->s * 0.5f);
		hsl.s = (hsl.l == 0 || hsl.l == 1) ? 0 : ((this->v - hsl.l) / Math::Min(hsl.l, 1.0f - hsl.l));
		return hsl;
	}
	constexpr HSV::operator CMYK() const
	{
		return (CMYK)(Color)(*this);
	}

	constexpr CMYK::operator Color() const
	{
#if HEPH_COLOR_CHANNEL_TYPE_I == 0
		return Color((1.0f - this->c) * (1.0f - this->k) * HEPH_COLOR_CHANNEL_MAX, (1.0f - this->m) * (1.0f - this->k) * HEPH_COLOR_CHANNEL_MAX, (1.0f - this->y) * (1.0f - this->k) * HEPH_COLOR_CHANNEL_MAX);
#else
		return Color(Math::Round((1.0f - this->c) * (1.0f - this->k) * HEPH_COLOR_CHANNEL_MAX), Math::Round((1.0f - this->m) * (1.0f - this->k) * HEPH_COLOR_CHANNEL_MAX), Math::Round((1.0f - this->y) * (1.0f - this->k) * HEPH_COLOR_CHANNEL_MAX));
#endif
	}
	constexpr CMYK::operator RGB() const
	{
		return (RGB)(Color)(*this);
	}
	constexpr CMYK::operator HSL() const
	{
		return (HSL)(Color)(*this);
	}
	constexpr CMYK::operator HSV() const
	{
		return (HSV)(Color)(*this);
	}

	class PredefinedColors final
	{
	public:
		static constexpr Color Red = Color(HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MAX);
		static constexpr Color Green = Color(HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MAX);
		static constexpr Color Blue = Color(HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX);
		static constexpr Color Magenta = Color(HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX);
		static constexpr Color Cyan = Color(HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX);
		static constexpr Color Yellow = Color(HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MAX);
		static constexpr Color Black = Color(HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MIN, HEPH_COLOR_CHANNEL_MAX);
		static constexpr Color White = Color(HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX);
		static constexpr Color Transparent = Color(HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MAX, HEPH_COLOR_CHANNEL_MIN);
	};
}