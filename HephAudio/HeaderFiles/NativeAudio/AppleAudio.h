#pragma once
#if defined(__APPLE__)
#include "HephAudioShared.h"
#include "NativeAudio.h"
#include <CoreAudio/CoreAudio.h>

namespace HephAudio
{
	namespace Native
	{
		// Uses CoreAudio, add -framework CoreFoundation and -framework CoreAudio to the compiler flags.
		class AppleAudio final : public NativeAudio
		{
		public:
			using NativeAudio::InitializeRender;
			using NativeAudio::InitializeCapture;
		private:
			AudioDeviceIOProcID renderProcID;
			AudioDeviceIOProcID captureProcID;
		public:
			AppleAudio();
			AppleAudio(const AppleAudio&) = delete;
			AppleAudio& operator=(const AppleAudio&) = delete;
			~AppleAudio();
			void SetMasterVolume(heph_float volume) override;
			heph_float GetMasterVolume() const override;
			void InitializeRender(AudioDevice* device, AudioFormatInfo format) override;
			void StopRendering() override;
			void InitializeCapture(AudioDevice* device, AudioFormatInfo format) override;
			void StopCapturing() override;
			void GetNativeParams(NativeAudioParams& nativeParams) const override;
			void SetNativeParams(const NativeAudioParams& nativeParams) override;
		private:
			bool EnumerateAudioDevices() override;
			void ToStreamDesc(const AudioFormatInfo& format, AudioStreamBasicDescription& streamDesc) const;
			void FromStreamDesc(AudioFormatInfo& format, const AudioStreamBasicDescription& streamDesc) const;
			static OSStatus RenderCallback(AudioDeviceID device, const AudioTimeStamp* now, const AudioBufferList* indata, const AudioTimeStamp* intime, AudioBufferList* outdata, const AudioTimeStamp* outtime, void* udata);
			static OSStatus CaptureCallback(AudioDeviceID device, const AudioTimeStamp* now, const AudioBufferList* indata, const AudioTimeStamp* intime, AudioBufferList* outdata, const AudioTimeStamp* outtime, void* udata);
		};
	}
}
#endif