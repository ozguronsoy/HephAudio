#include "HephAudioShared.h"

namespace HephAudio
{
	static const char* const __version = HEPHAUDIO_VERSION;

	const char* GetVersion()
	{
		return __version;
	}

	unsigned int GetVersionMajor()
	{
		return HEPHAUDIO_VERSION_MAJOR;
	}

	unsigned int GetVersionMinor()
	{
		return HEPHAUDIO_VERSION_MINOR;
	}

	unsigned int GetVersionPatch()
	{
		return HEPHAUDIO_VERSION_PATCH;
	}
}