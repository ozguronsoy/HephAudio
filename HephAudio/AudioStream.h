#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "AudioFileFormatManager.h"
#include "AudioObject.h"
#include "NativeAudio.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	class AudioStream final
	{
	private:
		static std::vector<AudioStream*> streams;
	private:
		Native::NativeAudio* pNativeAudio;
		HephCommon::File file;
		FileFormats::IAudioFileFormat* pFileFormat;
		Codecs::IAudioCodec* pAudioCodec;
		AudioFormatInfo formatInfo;
		std::shared_ptr<AudioObject> pao;
	public:
		AudioStream(Native::NativeAudio* pNativeAudio, HephCommon::StringBuffer filePath);
		AudioStream(const AudioStream&) = delete;
		AudioStream& operator=(const AudioStream&) = delete;
		~AudioStream();
		HephCommon::File* GetFile() noexcept;
		FileFormats::IAudioFileFormat* GetFileFormat() const noexcept;
		Codecs::IAudioCodec* GetAudioCodec() const noexcept;
		std::shared_ptr<AudioObject> GetAudioObject() const noexcept;
		const AudioFormatInfo& GetAudioFormatInfo() const noexcept;
		void Release() noexcept;
	private:
		void Release(bool destroyAO) noexcept;
	private:
		static void RemoveStream(AudioStream* pStream) noexcept;
		static void OnRender(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
		static void OnFinishedPlaying(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
	public:
		static AudioStream* FindStream(const AudioObject* pAudioObject);
	};
}