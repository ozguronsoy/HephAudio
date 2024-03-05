#pragma once
#include "HephCommonShared.h"
#include "File.h"
#include "StringBuffer.h"

namespace HephCommon
{
	class StreamHelpers final
	{
	public:
		StreamHelpers() = delete;
		StreamHelpers(const StreamHelpers&) = delete;
		StreamHelpers& operator=(const StreamHelpers&) = delete;
	public:
		static wchar_t NextChar(const File& file, StringType stringType);
		static bool FindChar(const File& file, StringType stringType, wchar_t c, StringBuffer* pOutChars);
		static bool FindString(const File& file, StringType stringType, StringBuffer str, StringBuffer* pOutChars);
		static size_t CharSize(StringType stringType);
	};
}