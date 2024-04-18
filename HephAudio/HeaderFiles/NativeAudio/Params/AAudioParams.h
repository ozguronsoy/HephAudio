#pragma once
#ifdef __ANDROID__
#include "HephAudioShared.h"
#include "NativeAudioParams.h"
#include <aaudio/AAudio.h>

namespace HephAudio
{
    namespace Native
    {
        struct AAudioParams final : public NativeAudioParams
        {
            aaudio_sharing_mode_t renderShareMode;
            aaudio_sharing_mode_t captureShareMode;
            aaudio_performance_mode_t renderPerformanceMode;
            aaudio_performance_mode_t capturePerformanceMode;
            heph_float renderBufferDuration_ms;
            heph_float captureBufferDuration_ms;

#if __ANDROID_API__ >= 28
            aaudio_content_type_t renderContentType;
            aaudio_content_type_t captureContentType;
            aaudio_usage_t renderUsage;
            aaudio_usage_t captureUsage;
#endif
            AAudioParams()
                : renderShareMode(AAUDIO_SHARING_MODE_SHARED), captureShareMode(AAUDIO_SHARING_MODE_SHARED)
                , renderPerformanceMode(AAUDIO_PERFORMANCE_MODE_NONE), capturePerformanceMode(AAUDIO_PERFORMANCE_MODE_NONE)
                , renderBufferDuration_ms(10), captureBufferDuration_ms(10)
#if __ANDROID_API__ >= 28
                , renderContentType(AAUDIO_CONTENT_TYPE_MUSIC), captureContentType(AAUDIO_CONTENT_TYPE_MUSIC)
                , renderUsage(AAUDIO_USAGE_MEDIA), captureUsage(AAUDIO_USAGE_MEDIA)
#endif
            {}
        };
    }
}
#endif