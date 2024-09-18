#pragma once
#if defined(__ANDROID__) && __ANDROID_API__ >= HEPHAUDIO_ANDROID_OPENSL_MIN_API_LEVEL
#include "HephAudioShared.h"
#include "NativeAudioParams.h"
#include <SLES/OpenSLES_Android.h>

/** @file */

namespace HephAudio
{
    namespace Native
    {
        /**
         * @brief struct for storing the OpenSL ES specific parameters.
         * 
         */
        struct OpenSLParams final : public NativeAudioParams
        {
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

			/** @copydoc default_constructor */
            OpenSLParams()
                : renderBufferDuration_ms(10), captureBufferDuration_ms(10) {}
        };
    }
}
#endif