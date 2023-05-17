#pragma once

#if !defined(CPP_VERSION)

#if defined(_MSVC_LANG)

#define CPP_VERSION _MSVC_LANG

#else

#define CPP_VERSION __cplusplus

#endif

#define CPP_VERSION_PRE_98 1L
#define CPP_VERSION_98 199711L
#define CPP_VERSION_11 201103L
#define CPP_VERSION_14 201402L
#define CPP_VERSION_17 201703L
#define CPP_VERSION_20 202002L

#endif



#if !defined(HEPH_FLOAT)

#if defined(HEPH_HIGH_PRECISION_FLOAT)
typedef double heph_float;
#else
typedef float heph_float;
#endif

#define HEPH_FLOAT heph_float
#endif


#if !defined(HEPH_CONSTEVAL)

#if CPP_VERSION >= CPP_VERSION_20
#define HEPH_CONSTEVAL consteval
#else
#define HEPH_CONSTEVAL constexpr
#endif

#endif


inline HEPH_CONSTEVAL heph_float operator""hf(unsigned long long int x)
{
	return (heph_float)x;
}

inline HEPH_CONSTEVAL heph_float operator""hf(long double x)
{
	return (heph_float)x;
}



enum class Endian : unsigned char
{
	Little = 0,
	Big = 1
};