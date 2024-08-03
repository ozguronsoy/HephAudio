#pragma once
#if defined(_WIN32)
#include "HephAudioShared.h"
#include "NativeAudioParams.h"
#include <audiopolicy.h>

namespace HephAudio
{
	namespace Native
	{
		struct WasapiParams final : public NativeAudioParams
		{
			tagCLSCTX renderClsCtx;
			tagCLSCTX captureClsCtx;
			AUDCLNT_SHAREMODE renderShareMode;
			AUDCLNT_SHAREMODE captureShareMode;
			DWORD renderStreamFlags;
			DWORD captureStreamFlags;
			double renderBufferDuration_ms;
			double captureBufferDuration_ms;
			double renderPeriodicity_ms;
			double capturePeriodicity_ms;
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