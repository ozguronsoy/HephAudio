#include "NativeAudio/WinAudioBase.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include <combaseapi.h>

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		WinAudioBase::WinAudioBase() : NativeAudio() {}
		void WinAudioBase::InitializeCOM() const
		{
			HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
			if (FAILED(hres))
			{
				if (hres == RPC_E_CHANGED_MODE)
				{
					hres = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
					if (!FAILED(hres))
					{
						return;
					}
				}
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(hres, "WinAudioBase::InitializeCOM", "An error occurred whilst initializing COM.", "Windows COM", _com_error(hres).ErrorMessage()));
			}
		}
		AudioFormatInfo WinAudioBase::WFX2AFI(const WAVEFORMATEX& wfx)
		{
			return AudioFormatInfo(wfx.wFormatTag, wfx.nChannels, wfx.wBitsPerSample, wfx.nSamplesPerSec);
		}
		WAVEFORMATEX WinAudioBase::AFI2WFX(const AudioFormatInfo& afi)
		{
			WAVEFORMATEX wfx{ 0 };
			wfx.wFormatTag = afi.formatTag;
			wfx.nChannels = afi.channelCount;
			wfx.nSamplesPerSec = afi.sampleRate;
			wfx.nAvgBytesPerSec = afi.ByteRate();
			wfx.nBlockAlign = afi.FrameSize();
			wfx.wBitsPerSample = afi.bitsPerSample;
			wfx.cbSize = 0;
			return wfx;
		}
	}
}