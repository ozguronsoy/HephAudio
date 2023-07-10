#pragma once
#include "HephAudioFramework.h"
#include "AudioBuffer.h"
#include "File.h"
#include "AudioFileFormatManager.h"
#include "AudioObject.h"
#include "NativeAudio.h"
#include "IAudioCodec.h"

namespace HephAudio
{
	class RealTimeAudioBuffer final
	{
	private:
		static std::vector<RealTimeAudioBuffer*> buffers;
	private:
		Native::NativeAudio* pNativeAudio;
		HephCommon::File* pFile;
		FileFormats::IAudioFileFormat* pFileFormat;
		Codecs::IAudioCodec* pAudioCodec;
		AudioFormatInfo formatInfo;
		std::shared_ptr<AudioObject> pao;
	public:
		RealTimeAudioBuffer(Native::NativeAudio* pNativeAudio, HephCommon::StringBuffer filePath);
		RealTimeAudioBuffer(const RealTimeAudioBuffer&) = delete;
		RealTimeAudioBuffer(RealTimeAudioBuffer&& rhs) noexcept;
		~RealTimeAudioBuffer();
		RealTimeAudioBuffer& operator=(const RealTimeAudioBuffer&) = delete;
		RealTimeAudioBuffer& operator=(RealTimeAudioBuffer&& rhs) noexcept;
		HephCommon::File* GetFile() const noexcept;
		FileFormats::IAudioFileFormat* GetFileFormat() const noexcept;
		Codecs::IAudioCodec* GetAudioCodec() const noexcept;
		std::shared_ptr<AudioObject> GetAudioObject() const noexcept;
		const AudioFormatInfo& GetAudioFormatInfo() const noexcept;
		void Release() noexcept;
		void Release(bool destroyAO) noexcept;
	private:
		static void RemoveFromVector(RealTimeAudioBuffer* pRtab) noexcept;
		static RealTimeAudioBuffer* FindRTAB(const AudioObject* pAudioObject);
		static void OnRender(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
		static void OnFinishedPlaying(HephCommon::EventArgs* pArgs, HephCommon::EventResult* pResult);
	};
}