#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "AudioObject.h"
#include "Audio.h"
#include "NativeAudio/NativeAudio.h"
#include "FFmpeg/FFmpegAudioDecoder.h"
#include "File.h"

#define HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY "audio_stream"

namespace HephAudio
{
	class AudioStream final
	{
	private:
		Native::NativeAudio* pNativeAudio;
		AudioFormatInfo formatInfo;
		size_t frameCount;
		AudioObject* pAudioObject;
		FFmpegAudioDecoder ffmpegAudioDecoder;
		AudioBuffer decodedBuffer;
	public:
		AudioStream(Native::NativeAudio* pNativeAudio);
		AudioStream(Audio& audio);
		AudioStream(Native::NativeAudio* pNativeAudio, const std::string& filePath);
		AudioStream(Audio& audio, const std::string& filePath);
		AudioStream(const AudioStream&) = delete;
		AudioStream(AudioStream&& rhs) noexcept;
		~AudioStream();
		AudioStream& operator=(const AudioStream&) = delete;
		AudioStream& operator=(AudioStream&& rhs) noexcept;
		Native::NativeAudio* GetNativeAudio() const;
		AudioObject* GetAudioObject() const;
		const AudioFormatInfo& GetAudioFormatInfo() const;
		size_t GetFrameCount() const;
		void ChangeFile(const std::string& newFilePath);
		void Start();
		void Stop();
		heph_float GetPosition() const;
		void SetPosition(heph_float position);
		void Release();
	private:
		void Release(bool destroyAO);
		static void OnRender(const HephCommon::EventParams& eventParams);
		static void OnFinishedPlaying(const HephCommon::EventParams& eventParams);
	};
}