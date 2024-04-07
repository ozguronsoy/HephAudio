#pragma once
#if defined(_WIN32)
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "StringHelpers.h"
#include <wrl.h>
#include <mmeapi.h>
#include <mmreg.h>
#include <comdef.h>

namespace HephAudio
{
	namespace Native
	{
		class WinAudioBase : public NativeAudio
		{
		public:
			WinAudioBase();
			WinAudioBase(const WinAudioBase&) = delete;
			WinAudioBase& operator=(const WinAudioBase&) = delete;
			virtual ~WinAudioBase() = default;
		protected:
			void InitializeCOM() const;
			static AudioFormatInfo WFX2AFI(const WAVEFORMATEXTENSIBLE& wfx);
			static WAVEFORMATEXTENSIBLE AFI2WFX(const AudioFormatInfo& afi);
			static std::string GetComErrorMessage(HRESULT hr);
		};
	}
}
#endif