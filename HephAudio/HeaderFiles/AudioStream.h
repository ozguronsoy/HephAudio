#pragma once
#include "HephAudioShared.h"
#include "Audio.h"
#include "AudioBuffer.h"
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
		double GetPosition() const;
		void SetPosition(double position);
		void Release();
	private:
		void Release(bool destroyAO);
		static void OnRender(const HephCommon::EventParams& eventParams);
		static void OnFinishedPlaying(const HephCommon::EventParams& eventParams);
	};
}