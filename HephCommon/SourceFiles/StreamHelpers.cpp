#include "StreamHelpers.h"

namespace HephCommon
{
	wchar_t StreamHelpers::NextChar(const File& file, StringType stringType)
	{
		wchar_t c = 0;
		file.Read(&c, StreamHelpers::CharSize(stringType), Endian::Little);
		return c;
	}
	bool StreamHelpers::FindChar(const File& file, StringType stringType, wchar_t c, StringBuffer* pOutChars)
	{
		wchar_t x = 0;
		do
		{
			file.Read(&x, StreamHelpers::CharSize(stringType), Endian::Little);

			if (pOutChars != nullptr) // get the chars before the target character. This is to get the element name or value between quotation marks
			{
				if (x == c)
				{
					return true;
				}
				(*pOutChars) += x;
			}

			if (file.GetOffset() == file.FileSize())
			{
				return false;
			}
		} while (x != c);
		return true;
	}
	bool StreamHelpers::FindString(const File& file, StringType stringType, StringBuffer str, StringBuffer* pOutChars)
	{
		wchar_t c;
		size_t matchingCharCount = 0;

		str.SetStringType(StringType::Wide);

		while (file.GetOffset() < file.FileSize())
		{
			c = StreamHelpers::NextChar(file, stringType);
			if (c == str.w_at(matchingCharCount))
			{
				matchingCharCount++;
				if (matchingCharCount == str.Size())
				{
					if (pOutChars != nullptr)
					{
						if (pOutChars->Size() + 1 > str.Size())
						{
							(*pOutChars) = pOutChars->SubString(0, pOutChars->Size() - str.Size() + 1);
						}
						else
						{
							(*pOutChars) = L"";
						}
					}
					return true;
				}
			}
			else
			{
				if (matchingCharCount > 0)
				{
					file.DecreaseOffset(StreamHelpers::CharSize(stringType)); // might still be the first character of the str, hence check again.
				}
				matchingCharCount = 0;
			}
			if (pOutChars != nullptr)
			{
				(*pOutChars) += c;
			}
		}

		return false;
	}
	size_t StreamHelpers::CharSize(StringType stringType)
	{
		return stringType == StringType::Wide ? sizeof(wchar_t) : sizeof(char);
	}
}