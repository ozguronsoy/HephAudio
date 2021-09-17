#include "IAudioObject.h"

namespace HephAudio
{
	namespace Structs
	{
		IAudioObject::IAudioObject()
		{
			filePath = L"";
			name = L"";
			paused = false;
			mute = false;
			reverse = false;
			constant = false;
			loopCount = 1;
			volume = 1.0;
			categories = std::vector<std::wstring>(0);
			distortionInfo = DistortionInfo();
		}
	}
}