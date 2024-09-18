#pragma once
#if defined(_WIN32)
#include "HephAudioShared.h"
#include "NativeAudioParams.h"
#include <audiopolicy.h>

/** @file */

namespace HephAudio
{
	namespace Native
	{
		/**
		 * @brief struct for storing the WASAPI specific parameters.
		 * 
		 */
		struct WasapiParams final : public NativeAudioParams
		{
			/**
			 * [render context](https://learn.microsoft.com/en-us/windows/win32/api/wtypesbase/ne-wtypesbase-clsctx).
			 * 
			 */
			tagCLSCTX renderClsCtx;

			/**
			 * [capture context](https://learn.microsoft.com/en-us/windows/win32/api/wtypesbase/ne-wtypesbase-clsctx).
			 * 
			 */
			tagCLSCTX captureClsCtx;

			/**
			 * [Render share mode](https://learn.microsoft.com/en-us/windows/win32/api/audiosessiontypes/ne-audiosessiontypes-audclnt_sharemode),
			 * <b>AUDCLNT_SHAREMODE_SHARED</b> or <b>AUDCLNT_SHAREMODE_EXCLUSIVE</b>.
			 * 
			 */
			AUDCLNT_SHAREMODE renderShareMode;

			/**
			 * [Capture share mode](https://learn.microsoft.com/en-us/windows/win32/api/audiosessiontypes/ne-audiosessiontypes-audclnt_sharemode), 
			 * <b>AUDCLNT_SHAREMODE_SHARED</b> or <b>AUDCLNT_SHAREMODE_EXCLUSIVE</b>.
			 * 
			 */
			AUDCLNT_SHAREMODE captureShareMode;

			/**
			 * [Render stream flags](https://learn.microsoft.com/en-us/windows/win32/coreaudio/audclnt-streamflags-xxx-constants).
			 * 
			 */
			DWORD renderStreamFlags;

			/**
			 * [Capture stream flags](https://learn.microsoft.com/en-us/windows/win32/coreaudio/audclnt-streamflags-xxx-constants).
			 * 
			 */
			DWORD captureStreamFlags;

			/**
			 * duration of the render buffer in milliseconds.
			 * 
			 */
			double renderBufferDuration_ms;

			/**
			 * duration of the capture buffer in milliseconds.
			 * 
			 */
			double captureBufferDuration_ms;

			/**
			 * periodicity of the render device in milliseconds. 
			 * This must be equal to the render buffer duration for exclusive streams.
			 * 
			 */
			double renderPeriodicity_ms;

			/**
			 * periodicity of the capture device in milliseconds. 
			 * This must be equal to the capture buffer duration for exclusive streams.
			 * 
			 */
			double capturePeriodicity_ms;

			/** @copydoc default_constructor */
			WasapiParams()
				: renderClsCtx(CLSCTX_INPROC_SERVER), captureClsCtx(CLSCTX_INPROC_SERVER)
				, renderShareMode(AUDCLNT_SHAREMODE_SHARED), captureShareMode(AUDCLNT_SHAREMODE_SHARED)
				, renderStreamFlags(AUDCLNT_STREAMFLAGS_EVENTCALLBACK), captureStreamFlags(0)
				, renderBufferDuration_ms(0), captureBufferDuration_ms(40)
				, renderPeriodicity_ms(0), capturePeriodicity_ms(0) {}
		};
	}
}
#endif