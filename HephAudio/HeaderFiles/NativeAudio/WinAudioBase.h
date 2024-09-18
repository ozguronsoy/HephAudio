#pragma once
#if defined(_WIN32)
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include "AudioEvents/AudioDeviceEventArgs.h"
#include "AudioEvents/AudioCaptureEventArgs.h"
#include "StringHelpers.h"
#include <wrl.h>
#include <mmeapi.h>
#include <mmreg.h>
#include <comdef.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief base class for classes that interact with the Windows audio APIs.
		 * 
		 */
		class WinAudioBase : public NativeAudio
		{
		public:
			/** @copydoc default_constructor */
			WinAudioBase();

			WinAudioBase(const WinAudioBase&) = delete;
			WinAudioBase& operator=(const WinAudioBase&) = delete;

			/** @copydoc destructor */
			virtual ~WinAudioBase() = default;

		protected:
			/**
			 * initializes COM.
			 * 
			 */
			void InitializeCOM() const;

			/**
			 * converts WAVEFORMATEXTENSIBLE to AudioFormatInfo.
			 * 
			 */
			static AudioFormatInfo WFX2AFI(const WAVEFORMATEXTENSIBLE& wfx);

			/**
			 * converts AudioFormatInfo to WAVEFORMATEXTENSIBLE.
			 * 
			 */
			static WAVEFORMATEXTENSIBLE AFI2WFX(const AudioFormatInfo& afi);

			/**
			 * gets the COM error message that corresponds to the HRESULT.
			 * 
			 */
			static std::string GetComErrorMessage(HRESULT hr);
		};
	}
}
#endif