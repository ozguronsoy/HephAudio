#pragma once
#include "HephAudioShared.h"
#include "AudioBuffer.h"
#include "AudioObject.h"
#include "Audio.h"
#include "NativeAudio/NativeAudio.h"
#include "AudioFormats/AudioFileFormatManager.h"
#include "AudioCodecs/IAudioCodec.h"
#include "FFmpeg/FFmpegAudioDecoder.h"
#include "File.h"

#define HEPHAUDIO_STREAM_EVENT_USER_ARG_KEY std::string("audio_stream")

namespace HephAudio
{
	class AudioStream final
	{
	private:
		Native::NativeAudio* pNativeAudio;
		HephCommon::File file;
		FileFormats::IAudioFileFormat* pFileFormat;
		Codecs::IAudioCodec* pAudioCodec;
		AudioFormatInfo formatInfo;
		size_t frameCount;
		AudioObject* pAudioObject;
#if defined(HEPHAUDIO_USE_FFMPEG)
		FFmpegAudioDecoder ffmpegAudioDecoder;
		AudioBuffer decodedBuffer;
#endif
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
		HephCommon::File* GetFile();
		FileFormats::IAudioFileFormat* GetFileFormat() const;
		Codecs::IAudioCodec* GetAudioCodec() const;
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