#pragma once
#include <cinttypes>

/** @file */

/** @def CPP_VERSION
 * targeted C++ version
 */

 /** @defgroup default_constructor
  * creates a new instance and initializes it with default values.
  *
 */

 /** @defgroup constructor
  * creates a new instance and initializes it with the provided values.
  *
 */

 /** @defgroup copy_constructor
  * creates a new instance and copies the rhs's data to it.
  *
  * @param rhs instance whose data will be copied.
 */

 /** @defgroup move_constructor
  * creates a new instance and moves the rhs's data to it.
  *
  * @param rhs instance whose data will be moved.
 */

 /** @defgroup destructor
  * releases the resources and destroys the instance.
  *
 */

 /**
  * converts \a x to string litteral without expanding the parameter.
 */
#define HEPH_STRINGIFY(x)	#x

 /**
  * converts \a x to string litteral with expanding the parameter.
 */
#define HEPH_TOSTRING(x)	HEPH_STRINGIFY(x)

#if defined(_MSVC_LANG)

#define CPP_VERSION _MSVC_LANG

#else

#define CPP_VERSION __cplusplus

#endif

#define CPP_VERSION_PRE_98	(1L)
#define CPP_VERSION_98		(199711L)
#define CPP_VERSION_11		(201103L)
#define CPP_VERSION_14		(201402L)
#define CPP_VERSION_17		(201703L)
#define CPP_VERSION_20		(202002L)
#define CPP_VERSION_23		(202101L)

#if CPP_VERSION < CPP_VERSION_17
#error C++ 17 or above is required
#endif

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

#if defined(_MSVC_LANG) || defined(__INTEL_COMPILER) || defined(__MINGW64__) || defined(__MINGW32__)
#define heph_aligned_malloc(size, alignment) _aligned_malloc(size, alignment)
#define heph_aligned_free _aligned_free
#else
#define heph_aligned_malloc(size, alignment) aligned_alloc(alignment, size)
#define heph_aligned_free free
#endif

#if (!defined(_MSC_VER) || defined(__INTEL_COMPILER))

#include <cstring>

#endif

 /**
  * exports/imports the members, methods, and classes for shared libraries.
  *
 */
#if defined(HEPH_SHARED_LIB)

#if defined(_WIN32)

#if defined(HEPH_EXPORTS)
#define HEPH_API __declspec(dllexport)
#else
#define HEPH_API __declspec(dllimport)

#endif

#else

#if defined(HEPH_EXPORTS)
#define HEPH_API __attribute__((visibility("default")))
#else
#define HEPH_API
#endif

#endif

#else

#define HEPH_API 

#endif

 /**
  * name of the current function.
  *
 */
#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
#define HEPH_FUNC __PRETTY_FUNCTION__
#elif defined(_MSVC_LANG)
#define HEPH_FUNC __FUNCSIG__
#else
#define HEPH_FUNC __func__
#endif

namespace Heph
{
	enum Endian : uint8_t
	{
		Little = 0x00,
		Big = 0x01,
		Unknown = 0xFF
	};

	/**
	 * endianness of the current system.
	 *
	 */
	extern HEPH_API Endian systemEndian;

	/**
	 * changes the endianness of the provided data.
	 *
	 * @param pData pointer to the data.
	 * @param dataSize size of the data in bytes.
	 */
	HEPH_API void ChangeEndian(uint8_t* pData, uint8_t dataSize);

	inline constexpr Endian operator!(const Endian& lhs)
	{
		switch (lhs)
		{
		case Endian::Little:
			return Endian::Big;
		case Endian::Big:
			return Endian::Little;
		default:
			return Endian::Unknown;
		}
	}

	/**
	 * endianness of the current system.
	 *
	 */
#define HEPH_SYSTEM_ENDIAN Heph::systemEndian

	 /**
	  * changes the endianness of the provided data.
	  *
	  */
#define HEPH_CHANGE_ENDIAN(pData, dataSize) Heph::ChangeEndian((uint8_t*)(pData), dataSize)

	enum ConvolutionMode
	{
		Full = 0,
		Central = 1,
		ValidPadding = 2
	};
}