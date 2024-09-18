#pragma once
#include "HephAudioShared.h"
#if defined(__linux__) && !defined(__ANDROID__)
#include "NativeAudioParams.h"

/** @file */

namespace HephAudio
{
    namespace Native
    {
        /**
         * @brief struct for storing the ALSA specific parameters.
         * 
         */
        struct AlsaParams final : public NativeAudioParams
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
            AlsaParams()
                : renderBufferDuration_ms(10), captureBufferDuration_ms(10) {}
        };
    }
}
#endif