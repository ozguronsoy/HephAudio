#if defined(_WIN32)
#include "NativeAudio/WinAudioBase.h"
#include "HephException.h"
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
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(hres, HEPH_FUNC, "An error occurred while initializing COM.", "Windows COM", WinAudioBase::GetComErrorMessage(hres)));
			}
		}
		AudioFormatInfo WinAudioBase::WFX2AFI(const WAVEFORMATEXTENSIBLE& wfx)
		{
			if (wfx.Format.cbSize != 0)
			{
				return AudioFormatInfo(wfx.SubFormat.Data1, wfx.Format.wBitsPerSample, 
					AudioChannelLayout(wfx.Format.nChannels, (AudioChannelMask)wfx.dwChannelMask), wfx.Format.nSamplesPerSec);
			}
			return AudioFormatInfo(wfx.Format.wFormatTag, wfx.Format.wBitsPerSample, AudioChannelLayout::DefaultChannelLayout(wfx.Format.nChannels), wfx.Format.nSamplesPerSec);
		}
		WAVEFORMATEXTENSIBLE WinAudioBase::AFI2WFX(const AudioFormatInfo& afi)
		{
			WAVEFORMATEXTENSIBLE wfx{ 0 };
			wfx.Format.wFormatTag = HEPHAUDIO_FORMAT_TAG_EXTENSIBLE;
			wfx.Format.nChannels = afi.channelLayout.count;
			wfx.dwChannelMask = (uint32_t)afi.channelLayout.mask;
			wfx.Format.nSamplesPerSec = afi.sampleRate;
			wfx.Format.nAvgBytesPerSec = afi.ByteRate();
			wfx.Format.nBlockAlign = afi.FrameSize();
			wfx.Format.wBitsPerSample = afi.bitsPerSample;
			wfx.Format.cbSize = 22;
			wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;
			wfx.SubFormat.Data1 = afi.formatTag;
			wfx.SubFormat.Data2 = 0;
			wfx.SubFormat.Data2 = 0;
			wfx.SubFormat.Data3 = 0x10;
			wfx.SubFormat.Data4[0] = 0x80;
			wfx.SubFormat.Data4[1] = 0;
			wfx.SubFormat.Data4[2] = 0;
			wfx.SubFormat.Data4[3] = 0xAA;
			wfx.SubFormat.Data4[4] = 0;
			wfx.SubFormat.Data4[5] = 0x38;
			wfx.SubFormat.Data4[6] = 0x9B;
			wfx.SubFormat.Data4[7] = 0x71;

			return wfx;
		}
		std::string WinAudioBase::GetComErrorMessage(HRESULT hr)
		{
			if (sizeof(TCHAR) == sizeof(wchar_t))
			{
				return StringHelpers::WideToStr((wchar_t*)_com_error(hr).ErrorMessage());
			}
			return (char*)_com_error(hr).ErrorMessage();
		}
	}
}
#endif