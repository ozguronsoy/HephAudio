#pragma once
#include "HephAudioShared.h"
#if defined(__ANDROID__) && __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL
#include "NativeAudioParams.h"
#include <aaudio/AAudio.h>

/** @file */

namespace HephAudio
{
    namespace Native
    {
        /**
         * @brief struct for storing the AAudio specific parameters.
         * 
         */
        struct HEPH_API AAudioParams final : public NativeAudioParams
        {
            /**
             * render share mode, <b>AAUDIO_SHARING_MODE_SHARED</b> or <b>AAUDIO_SHARING_MODE_EXCLUSIVE</b>.
             * 
             */
            aaudio_sharing_mode_t renderShareMode;

            /**
             * capture share mode, <b>AAUDIO_SHARING_MODE_SHARED</b> or <b>AAUDIO_SHARING_MODE_EXCLUSIVE</b>.
             * 
             */
            aaudio_sharing_mode_t captureShareMode;

            /**
             * render performance mode.
             * 
             */
            aaudio_performance_mode_t renderPerformanceMode;

            /**
             * capture performance mode.
             * 
             */
            aaudio_performance_mode_t capturePerformanceMode;

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

#if __ANDROID_API__ >= 28
            /**
             * render content type, available for API level 28 and above.
             * 
             */
            aaudio_content_type_t renderContentType;

            /**
             * capture content type, available for API level 28 and above.
             * 
             */
            aaudio_content_type_t captureContentType;

            /**
             * render usage type, available for API level 28 and above.
             * 
             */
            aaudio_usage_t renderUsage;

            /**
             * capture usage type, available for API level 28 and above.
             * 
             */
            aaudio_usage_t captureUsage;
#endif

			/** @copydoc default_constructor */            
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