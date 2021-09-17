#include "AudioException.h"
#include <exception>
#include <sstream>
#include <iomanip>

namespace HephAudio
{
	namespace Structs
	{
		AudioException::AudioException()
		{
			this->hr = S_OK;
			this->method = L"";
			this->message = L"";
		}
		AudioException::AudioException(HRESULT hr, std::wstring method, std::wstring message)
			: std::exception(("Audio Exception " + HRToHex(hr) + " (" + std::to_string(hr) + ")\nMethod: " + std::string(method.begin(), method.end()) + "\nMessage: " + std::string(message.begin(), message.end())).c_str())
		{
			this->hr = hr;
			this->method = method;
			this->message = message;
		}
		std::string AudioException::HRToHex(HRESULT hr) const
		{
			std::stringstream ss;
			ss << "0x" << std::setfill('0') << std::setw(sizeof(HRESULT) * 2) << std::hex << hr;
			return ss.str();
		}
	}
}