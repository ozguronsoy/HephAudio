#pragma once
#include "HephAudioShared.h"
#if defined(__linux__) && !defined(__ANDROID__)
#include "NativeAudioParams.h"

namespace HephAudio
{
    namespace Native
    {
        struct AlsaParams final : public NativeAudioParams
        {
            double renderBufferDuration_ms;
            double captureBufferDuration_ms;
            AlsaParams()
                : renderBufferDuration_ms(10), captureBufferDuration_ms(10) {}
        };
    }
}
#endif