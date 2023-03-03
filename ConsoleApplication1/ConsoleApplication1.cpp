#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>
#include <ConsoleLogger.h>
#include <StopWatch.h>
#include <shlobj.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnDeviceAdded(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnDeviceRemoved(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnRender(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnFinishedPlaying(AudioEventArgs* pArgs, AudioEventResult* pResult);
HEPHAUDIO_DOUBLE PrintDeltaTime(StringBuffer label);
int Run(Audio& audio, StringBuffer& audioPath);

int main()
{
	wchar_t desktopPath[MAX_PATH + 1];
	SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);
	StringBuffer audioPath = desktopPath;
	audioPath.SetStringType(StringType::Normal);
	audioPath += '\\';

	Audio audio = Audio();
	audio.SetOnExceptionHandler(OnException);
	audio.SetOnAudioDeviceAddedHandler(OnDeviceAdded);
	audio.SetOnAudioDeviceRemovedHandler(OnDeviceRemoved);

	audio.InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));

	return Run(audio, audioPath);
}
void OnException(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	AudioException& ex = ((AudioExceptionEventArgs*)pArgs)->exception;
	AudioExceptionThread& t = ((AudioExceptionEventArgs*)pArgs)->thread;

	std::string str = ("[" + AudioExceptionThreadName(t) + "] " + (char*)ex).fc_str();
	size_t pos = str.find('\n', 0);
	str.insert(pos + 1, 21, ' ');
	pos = str.find('\n', pos + 1);
	str.insert(pos + 1, 21, ' ');
	ConsoleLogger::LogLine(str.c_str(), ConsoleLogger::error);
}
void OnDeviceAdded(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	NativeAudio* pNativeAudio = (NativeAudio*)pArgs->pNativeAudio;
	AudioDeviceEventArgs* pDeviceArgs = (AudioDeviceEventArgs*)pArgs;

	if (pDeviceArgs->audioDevice.type == AudioDeviceType::Render && pDeviceArgs->audioDevice.isDefault)
	{
		pNativeAudio->InitializeRender(&pDeviceArgs->audioDevice, pNativeAudio->GetRenderFormat());
	}
}
void OnDeviceRemoved(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	NativeAudio* pNativeAudio = (NativeAudio*)pArgs->pNativeAudio;
	AudioDeviceEventArgs* pDeviceArgs = (AudioDeviceEventArgs*)pArgs;

	if (pDeviceArgs->audioDevice.id == pNativeAudio->GetRenderDevice().id)
	{
		pNativeAudio->InitializeRender(nullptr, pNativeAudio->GetRenderFormat());
	}
}
void OnRender(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	NativeAudio* pNativeAudio = (NativeAudio*)pArgs->pNativeAudio;
	AudioObject* pAudioObject = (AudioObject*)pArgs->pAudioObject;
	AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
	AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

	const size_t readFrameCount = (HEPHAUDIO_DOUBLE)pRenderArgs->renderFrameCount * pAudioObject->buffer.FormatInfo().sampleRate / pNativeAudio->GetRenderFormat().sampleRate;

	pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, readFrameCount);

	AudioProcessor::ConvertSampleRate(pRenderResult->renderBuffer, pNativeAudio->GetRenderFormat().sampleRate, pRenderArgs->renderFrameCount);

	pAudioObject->frameIndex += readFrameCount;
	pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
}
void OnFinishedPlaying(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	AudioObject* pAudioObject = (AudioObject*)pArgs->pAudioObject;
	AudioFinishedPlayingEventArgs* pFinishedPlayingArgs = (AudioFinishedPlayingEventArgs*)pArgs;

	if (pFinishedPlayingArgs->remainingLoopCount > 0)
	{
		ConsoleLogger::LogLine("The track \"" + pAudioObject->name + "\" is starting over, number of remaning loops: " + StringBuffer::ToString(pFinishedPlayingArgs->remainingLoopCount), ConsoleLogger::debug);
	}
}
HEPHAUDIO_DOUBLE PrintDeltaTime(StringBuffer label)
{
	const HEPHAUDIO_DOUBLE dt = StopWatch::DeltaTime(StopWatch::milli);
	label = label + " " + StringBuffer::ToString(dt, 4);
	label += " ms";
	ConsoleLogger::LogLine(label, ConsoleLogger::success);
	StopWatch::Reset();
	return dt;
}
int Run(Audio& audio, StringBuffer& audioPath)
{
	std::string a;
	StringBuffer sb = "";
	while (sb != "exit" && sb != "quit")
	{
		std::getline(std::cin, a);
		sb = a.c_str();
		if (sb.Contains("path"))
		{
			if (sb == "path")
			{
				ConsoleLogger::LogLine(audioPath, ConsoleLogger::info);
			}
			else
			{
				audioPath = sb.Split('"').at(1);
			}
		}
		else if (sb.Contains("play"))
		{
			StringBuffer arg1 = sb.Split(' ').at(1);
			bool isNumber = true;
			for (size_t i = 0; i < arg1.Size(); i++)
			{
				if (!isdigit(arg1.c_str()[i]))
				{
					isNumber = false;
					break;
				}
			}
			std::shared_ptr<AudioObject> pao = audio.Play(audioPath + sb.Split('\"').at(1), isNumber ? StringBuffer::HexStringToUI32(arg1) : 1);
			pao->OnRender = OnRender;
			pao->OnFinishedPlaying = OnFinishedPlaying;
		}
		else if (sb.Contains("load"))
		{
			StopWatch::Reset();
			audio.Load(audioPath + sb.Split('\"').at(1))->OnFinishedPlaying = OnFinishedPlaying;
			PrintDeltaTime("File loaded in");
		}
		else if (sb.Contains("volume"))
		{
			HEPHAUDIO_DOUBLE volume = StringBuffer::StringToDouble(sb.SubString(sb.Find(' '), 10));
			audio.GetAO("", 0)->volume = volume;
		}
		else if (sb.Contains("position"))
		{
			HEPHAUDIO_DOUBLE position = StringBuffer::StringToDouble(sb.SubString(sb.Find(' '), 10));
			audio.SetAOPosition(audio.GetAO("", 0), position);
		}
		else if (sb == "pause")
		{
			audio.GetAO("", 0)->pause = true;
		}
		else if (sb == "resume")
		{
			audio.GetAO("", 0)->pause = false;
		}
		else if (sb == "reverse")
		{
			StopWatch::Reset();
			AudioProcessor::Reverse(audio.GetAO("", 0)->buffer);
			PrintDeltaTime("reverse applied in");
		}
		else if (sb == "normalize")
		{
			StopWatch::Reset();
			AudioProcessor::Normalize(audio.GetAO("", 0)->buffer, 1.0);
			PrintDeltaTime("normalized in");
		}
		else if (sb.Contains("distortion"))
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			std::vector<StringBuffer> params = sb.Split(' ');
			HEPHAUDIO_DOUBLE distortionParam = StringBuffer::StringToDouble(params.at(2));
			if (params.at(1) == "hc")
			{
				StopWatch::Reset();
				AudioProcessor::HardClipDistortion(pao->buffer, distortionParam);
				PrintDeltaTime("hard-clipping distortion applied in");
			}
			else if (params.at(1) == "sc")
			{
				StopWatch::Reset();
				AudioProcessor::SoftClipDistortion(pao->buffer, distortionParam);
				PrintDeltaTime("soft-clipping distortion applied in");
			}
			else if (params.at(1) == "od")
			{
				StopWatch::Reset();
				AudioProcessor::Overdrive(pao->buffer, distortionParam);
				PrintDeltaTime("overdrive applied in");
			}
			else if (params.at(1) == "fz")
			{
				HEPHAUDIO_DOUBLE alpha = StringBuffer::StringToDouble(params.at(3));
				StopWatch::Reset();
				AudioProcessor::Fuzz(pao->buffer, distortionParam, alpha);
				PrintDeltaTime("fuzz applied in");
			}
		}
		else if (sb.Contains("flanger"))
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			std::vector<StringBuffer> params = sb.Split(' ');
			HEPHAUDIO_DOUBLE depth = StringBuffer::StringToDouble(params.at(1));
			HEPHAUDIO_DOUBLE delay = StringBuffer::StringToDouble(params.at(2));
			HEPHAUDIO_DOUBLE rate = StringBuffer::StringToDouble(params.at(3));
			HEPHAUDIO_DOUBLE phase = StringBuffer::StringToDouble(params.at(4));
			const bool originalState = pao->pause;

			pao->pause = true;
			StopWatch::Reset();
			AudioProcessor::Flanger(pao->buffer, depth, delay, rate, phase);
			PrintDeltaTime("flanger applied in");
			pao->pause = originalState;
		}
		else if (sb.Contains("filter"))
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			std::vector<StringBuffer> params = sb.Split(' ');
			HEPHAUDIO_DOUBLE f1 = StringBuffer::StringToDouble(params.at(2));
			HEPHAUDIO_DOUBLE f2 = params.size() > 3 ? StringBuffer::StringToDouble(params.at(3)) : 0.0;

			if (params.at(1) == "lp")
			{
				StopWatch::Reset();
				AudioProcessor::LowPassFilterMT(pao->buffer, 512, 1024, f1);
				PrintDeltaTime("low-pass filter applied in");
			}
			else if (params.at(1) == "hp")
			{
				StopWatch::Reset();
				AudioProcessor::HighPassFilterMT(pao->buffer, 512, 1024, f1);
				PrintDeltaTime("high-pass filter applied in");
			}
			else if (params.at(1) == "bp")
			{
				StopWatch::Reset();
				AudioProcessor::BandPassFilterMT(pao->buffer, 512, 1024, f1, f2);
				PrintDeltaTime("band-pass filter applied in");
			}
			else if (params.at(1) == "bc")
			{
				StopWatch::Reset();
				AudioProcessor::BandCutFilterMT(pao->buffer, 512, 1024, f1, f2);
				PrintDeltaTime("band-cut filter applied in");
			}
		}
		else if (sb == "original")
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			const bool originalState = pao->pause;
			pao->pause = true;
			pao->buffer.Resize(0);
			StopWatch::Reset();
			AudioFile audioFile = AudioFile(pao->filePath);
			Formats::IAudioFormat* audioFormat = audio.GetAudioFormats()->GetAudioFormat(pao->filePath);
			audioFormat->ReadFile(audioFile, pao->buffer);
			pao->buffer.SetFormat(audio.GetRenderFormat());
			PrintDeltaTime("removed all effects in");
			pao->pause = originalState;
		}
	}

	return 0;
}