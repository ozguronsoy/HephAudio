#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "AudioFileFormatManager.h"
#include "AudioObject.h"
#include "NativeAudio.h"
#include "IAudioCodec.h"
#include "Audio.h"

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
		AudioObject* pAudioObject;
	public:
		AudioStream(Native::NativeAudio* pNativeAudio, HephCommon::StringBuffer filePath);
		AudioStream(Audio& audio, HephCommon::StringBuffer filePath);
		AudioStream(const AudioStream&) = delete;
		AudioStream(AudioStream&& rhs) noexcept;
		AudioStream& operator=(const AudioStream&) = delete;
		AudioStream& operator=(AudioStream&& rhs) noexcept;
		~AudioStream();
		HephCommon::File* GetFile() ;
		FileFormats::IAudioFileFormat* GetFileFormat() const ;
		Codecs::IAudioCodec* GetAudioCodec() const ;
		AudioObject* GetAudioObject() const ;
		const AudioFormatInfo& GetAudioFormatInfo() const ;
		void Release() ;
	private:
		void Release(bool destroyAO) ;
	private:
		static void RemoveStream(AudioStream* pStream) ;
		static void OnRender(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
		static void OnFinishedPlaying(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
	public:
		static AudioStream* FindStream(const AudioObject* pAudioObject);
	};
}