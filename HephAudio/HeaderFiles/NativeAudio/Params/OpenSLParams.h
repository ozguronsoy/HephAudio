#pragma once
#if defined(__ANDROID__) && __ANDROID_API__ >= HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL
#include "HephAudioShared.h"
#include "NativeAudioParams.h"
#include <SLES/OpenSLES_Android.h>

namespace HephAudio
{
    namespace Native
    {
        struct OpenSLParams final : public NativeAudioParams
        {
            double renderBufferDuration_ms;
            double captureBufferDuration_ms;
            OpenSLParams()
                : renderBufferDuration_ms(10), captureBufferDuration_ms(10) {}
        };
    }
}
#endif