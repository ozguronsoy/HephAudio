#pragma once
#include "HephAudioFramework.h"
#include "NativeAudio.h"

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
		};
	}
}