#pragma once
#include <cinttypes>

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
#define CPP_VERSION_23 202101L

#if CPP_VERSION < CPP_VERSION_14
#error C++ 14 or above is required
#endif

#endif

#if !defined(HEPH_ENV_64_BIT) && !defined(HEPH_ENV_32_BIT)

#if defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(_WIN32))

#if defined(_M_X64) || defined(_WIN64)
#define HEPH_ENV_64_BIT
#else
#define HEPH_ENV_32_BIT
#endif

#elif defined(__clang__) || defined(__INTEL_COMPILER) || defined(__GNUC__)
#if defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
#define HEPH_ENV_64_BIT
#else
#define HEPH_ENV_32_BIT
#endif

#else

#error unsupported environment

#endif

#endif

#if !defined(HEPH_CONSTEVAL)

#if CPP_VERSION >= CPP_VERSION_20
#define HEPH_CONSTEVAL consteval
#else
#define HEPH_CONSTEVAL constexpr
#endif

#endif

#if !defined(heph_aligned_malloc)

#if defined(_MSVC_LANG) || defined(__INTEL_COMPILER)
#define heph_aligned_malloc(size, alignment) _aligned_malloc(size, alignment)
#define heph_aligned_free _aligned_free
#else
#define heph_aligned_malloc(size, alignment) aligned_alloc(alignment, size)
#define heph_aligned_free free
#endif

#endif

#if (!defined(_MSC_VER) || defined(__INTEL_COMPILER))

#include <cstring>

#endif


namespace HephCommon
{
#if !defined(HEPH_ENDIAN)

	enum Endian : uint8_t
	{
		Little = 0x00,
		Big = 0x01,
		Unknown = 0xFF
	};
	extern Endian systemEndian;
	void ChangeEndian(uint8_t* pData, uint8_t dataSize);
	constexpr inline Endian operator!(const Endian& lhs) { return lhs & Endian::Big ? Endian::Little : (lhs == Endian::Little ? Endian::Big : Endian::Unknown); }

#define HEPH_ENDIAN HephCommon::Endian
#define HEPH_SYSTEM_ENDIAN HephCommon::systemEndian
#define HEPH_CHANGE_ENDIAN(pData, dataSize) HephCommon::ChangeEndian(pData, dataSize)

#endif

#if !defined(HEPH_CONVOLUTION_MODE)

	enum ConvolutionMode
	{
		Full = 0,
		Central = 1,
		ValidPadding = 2
	};
#define HEPH_CONVOLUTION_MODE HephCommon::ConvolutionMode

#endif
}