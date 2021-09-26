#include "FlacFormat.h"
#include "AudioException.h"

namespace HephAudio
{
	namespace Formats
	{
		std::wstring FlacFormat::Extension() const noexcept
		{
			return L".flac";
		}
		AudioBuffer FlacFormat::ReadFile(AudioFile& file) const
		{
			throw AudioException(E_NOTIMPL, L"FlacFormat::ReadFile", L"Not implemented.");
		}
		bool FlacFormat::SaveToFile(std::wstring filePath, AudioBuffer& buffer, bool overwrite) const
		{
			throw AudioException(E_NOTIMPL, L"FlacFormat::SaveToFile", L"Not implemented.");
		}
	}
}