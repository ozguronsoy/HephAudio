#pragma once
#include "framework.h"
#include <string>

namespace HephAudio
{
	namespace Structs
	{
		struct AudioException
		{
			long hr;
			std::wstring method; // Method name that exception has occurred.
			std::wstring message;
			AudioException();
			AudioException(long hr, std::wstring method, std::wstring message);
			virtual ~AudioException() = default;
			virtual std::string What() const;
			virtual std::wstring WhatW() const;
		protected:
			virtual std::string HRToHex(long hr) const;
			virtual std::wstring HRToHexW(long hr) const;
		};
	}
}