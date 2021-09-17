#pragma once
#include "framework.h"
#include <string>

namespace HephAudio
{
	namespace Structs
	{
		struct HephAudioAPI AudioException : public std::exception
		{
			HRESULT hr;
			std::wstring method; // Method name that exception has occurred.
			std::wstring message;
			AudioException();
			AudioException(HRESULT hr, std::wstring method, std::wstring message);
			virtual ~AudioException() = default;
		protected:
			virtual std::string HRToHex(HRESULT hr) const;
		};
	}
}