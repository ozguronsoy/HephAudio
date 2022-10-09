#include "AudioException.h"
#include <exception>
#include <sstream>
#include <iomanip>

namespace HephAudio
{
	AudioException::AudioException()
	{
		this->hr = 0;
		this->method = L"";
		this->message = L"";
	}
	AudioException::AudioException(long hr, std::wstring method, std::wstring message)
	{
		this->hr = hr;
		this->method = method;
		this->message = message;
	}
	std::string AudioException::HRToHex(long hr) const
	{
		std::stringstream ss;
		ss << "0x" << std::setfill('0') << std::setw(sizeof(long) * 2) << std::hex << hr;
		return ss.str();
	}
	std::wstring AudioException::HRToHexW(long hr) const
	{
		std::wstringstream ss;
		ss << L"0x" << std::setfill(L'0') << std::setw(sizeof(long) * 2) << std::hex << hr;
		return ss.str();
	}
	std::string AudioException::What() const
	{
		return ("Audio Exception " + HRToHex(hr) + " (" + std::to_string(hr) + ")\nMethod: " + std::string(method.begin(), method.end()) + "\nMessage: " + std::string(message.begin(), message.end()));
	}
	std::wstring AudioException::WhatW() const
	{
		return L"Audio Exception " + HRToHexW(hr) + L" (" + std::to_wstring(hr) + L")\nMethod: " + method + L"\nMessage: " + message;
	}
}