#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>
#include <ConsoleLogger.h>
#include <StopWatch.h>
#include "FloatBuffer.h"
#include <shlobj.h>
#include "AudioFileFormatManager.h"
#include "AudioCodecManager.h"
#include <RealTimeAudioBuffer.h>
#include <WavFileMetadataReader.h>

using namespace HephCommon;
using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(EventArgs* pArgs, EventResult* pResult);
void OnDeviceAdded(EventArgs* pArgs, EventResult* pResult);
void OnDeviceRemoved(EventArgs* pArgs, EventResult* pResult);
void OnRender(EventArgs* pArgs, EventResult* pResult);
void OnFinishedPlaying(EventArgs* pArgs, EventResult* pResult);
heph_float PrintDeltaTime(StringBuffer label);
int Run(Audio& audio, StringBuffer& audioRoot);

int main()
{
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD outMode = 0;
	GetConsoleMode(stdoutHandle, &outMode);
	
	SetConsoleMode(stdoutHandle, outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	wchar_t desktopPath[MAX_PATH + 1];
	SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);
	StringBuffer audioRoot = desktopPath;
	audioRoot += '\\';
	audioRoot += "AudioFiles\\";

	HephException::OnException = OnException;

	Audio audio = Audio();
	audio->OnAudioDeviceAdded = OnDeviceAdded;
	audio->OnAudioDeviceRemoved = OnDeviceRemoved;
	audio->SetDeviceEnumerationPeriod(250e6);

	audio.InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48e3));

	return Run(audio, audioRoot);
}
void OnException(EventArgs* pArgs, EventResult* pResult)
{
	const HephException& ex = ((HephExceptionEventArgs*)pArgs)->exception;

	StringBuffer exceptionString = ex.method + " (" + StringBuffer::ToHexString(ex.errorCode) + ")\n\t\t" + ex.message;
	ConsoleLogger::Log(exceptionString, ConsoleLogger::error);
}
void OnDeviceAdded(EventArgs* pArgs, EventResult* pResult)
{
	AudioDeviceEventArgs* pDeviceArgs = (AudioDeviceEventArgs*)pArgs;
	NativeAudio* pNativeAudio = (NativeAudio*)pDeviceArgs->pNativeAudio;

	if (pDeviceArgs->audioDevice.type == AudioDeviceType::Render && pDeviceArgs->audioDevice.isDefault)
	{
		pNativeAudio->StopRendering();
		pNativeAudio->InitializeRender(&pDeviceArgs->audioDevice, pNativeAudio->GetRenderFormat());
	}
}
void OnDeviceRemoved(EventArgs* pArgs, EventResult* pResult)
{
	AudioDeviceEventArgs* pDeviceArgs = (AudioDeviceEventArgs*)pArgs;
	NativeAudio* pNativeAudio = (NativeAudio*)pDeviceArgs->pNativeAudio;

	if (pDeviceArgs->audioDevice.id == pNativeAudio->GetRenderDevice().id)
	{
		pNativeAudio->StopRendering();
		pNativeAudio->InitializeRender(nullptr, pNativeAudio->GetRenderFormat());
	}
}
void OnRender(EventArgs* pArgs, EventResult* pResult)
{
	AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
	AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;
	NativeAudio* pNativeAudio = (NativeAudio*)pRenderArgs->pNativeAudio;
	AudioObject* pAudioObject = (AudioObject*)pRenderArgs->pAudioObject;

	const size_t readFrameCount = (heph_float)pRenderArgs->renderFrameCount * (heph_float)pAudioObject->buffer.FormatInfo().sampleRate / (heph_float)pNativeAudio->GetRenderFormat().sampleRate;

	pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, readFrameCount);

	AudioProcessor::ConvertSampleRateRT(pAudioObject->buffer, pRenderResult->renderBuffer, pAudioObject->frameIndex, pNativeAudio->GetRenderFormat().sampleRate, pRenderArgs->renderFrameCount);

	pAudioObject->frameIndex += readFrameCount;
	pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
}
void OnFinishedPlaying(EventArgs* pArgs, EventResult* pResult)
{
	AudioFinishedPlayingEventArgs* pFinishedPlayingArgs = (AudioFinishedPlayingEventArgs*)pArgs;
	AudioObject* pAudioObject = (AudioObject*)pFinishedPlayingArgs->pAudioObject;

	if (pFinishedPlayingArgs->remainingLoopCount > 0)
	{
		ConsoleLogger::Log("The track \"" + pAudioObject->name + "\" is starting over, number of remaning loops: " + StringBuffer::ToString(pFinishedPlayingArgs->remainingLoopCount), ConsoleLogger::debug);
	}
}
heph_float PrintDeltaTime(StringBuffer label)
{
	const heph_float dt = StopWatch::StaticDeltaTime(StopWatch::milli);
	label = label + " " + StringBuffer::ToString(dt, 4);
	label += " ms";
	ConsoleLogger::Log(label, ConsoleLogger::success);
	StopWatch::StaticReset();
	return dt;
}
int Run(Audio& audio, StringBuffer& audioRoot)
{
	std::wstring a;
	StringBuffer sb = "";
	while (sb != L"exit" && sb != L"quit")
	{
		try
		{
		std:getline(std::wcin, a);
			sb = a.c_str();
			if (sb.CompareContent("")) continue;
			if (sb.Contains("path"))
			{
				if (sb == L"path")
				{
					ConsoleLogger::Log(audioRoot, ConsoleLogger::info);
				}
				else
				{
					audioRoot = sb.Split('"').at(1);
				}
			}
			else if (sb.Contains("play"))
			{
				StringBuffer arg1 = sb.Split(' ').at(1);
				bool isNumber = true;
				for (size_t i = 0; i < arg1.Size(); i++)
				{
					if (!isdigit(arg1.wc_str()[i]))
					{
						isNumber = false;
						break;
					}
				}
				StopWatch::StaticReset();
				std::shared_ptr<AudioObject> pao = audio.Play(audioRoot + sb.Split('\"').at(1), isNumber ? StringBuffer::HexStringToUI32(arg1) : 1);
				pao->OnRender = OnRender;
				pao->OnFinishedPlaying = OnFinishedPlaying;
				PrintDeltaTime("Play function executed in");
			}
			else if (sb.Contains("load"))
			{
				StopWatch::StaticReset();
				std::shared_ptr<AudioObject> pao = audio.Load(audioRoot + sb.Split('\"').at(1));
				pao->OnFinishedPlaying = OnFinishedPlaying;
				PrintDeltaTime("File loaded in");
			}
			else if (sb.Contains("volume"))
			{
				heph_float volume = StringBuffer::StringToDouble(sb.SubString(sb.Find(' '), 10));
				audio.GetAO("", 0)->volume = volume;
			}
			else if (sb.Contains("position"))
			{
				heph_float position = StringBuffer::StringToDouble(sb.SubString(sb.Find(' '), 10));
				audio.SetAOPosition(audio.GetAO("", 0), position);
			}
			else if (sb == L"pause")
			{
				audio.GetAO("", 0)->pause = true;
			}
			else if (sb == L"resume")
			{
				audio.GetAO("", 0)->pause = false;
			}
			else if (sb == L"reverse")
			{
				StopWatch::StaticReset();
				AudioProcessor::Reverse(audio.GetAO("", 0)->buffer);
				PrintDeltaTime("reverse applied in");
			}
			else if (sb == L"normalize")
			{
				StopWatch::StaticReset();
				AudioProcessor::Normalize(audio.GetAO("", 0)->buffer, 1.0);
				PrintDeltaTime("normalized in");
			}
			else if (sb.Contains("distortion"))
			{
				std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
				std::vector<StringBuffer> params = sb.Split(' ');
				heph_float distortionParam = StringBuffer::StringToDouble(params.at(2));
				if (params.at(1) == L"hc")
				{
					StopWatch::StaticReset();
					AudioProcessor::HardClipDistortion(pao->buffer, distortionParam);
					PrintDeltaTime("hard-clipping distortion applied in");
				}
				else if (params.at(1) == L"sc")
				{
					StopWatch::StaticReset();
					AudioProcessor::SoftClipDistortion(pao->buffer, distortionParam);
					PrintDeltaTime("soft-clipping distortion applied in");
				}
				else if (params.at(1) == L"od")
				{
					StopWatch::StaticReset();
					AudioProcessor::Overdrive(pao->buffer, distortionParam);
					PrintDeltaTime("overdrive applied in");
				}
				else if (params.at(1) == L"fz")
				{
					heph_float alpha = StringBuffer::StringToDouble(params.at(3));
					StopWatch::StaticReset();
					AudioProcessor::Fuzz(pao->buffer, distortionParam, alpha);
					PrintDeltaTime("fuzz applied in");
				}
			}
			else if (sb.Contains("flanger"))
			{
				std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
				std::vector<StringBuffer> params = sb.Split(' ');
				heph_float depth = StringBuffer::StringToDouble(params.at(1));
				heph_float delay = StringBuffer::StringToDouble(params.at(2));
				heph_float rate = StringBuffer::StringToDouble(params.at(3));
				heph_float phase = StringBuffer::StringToDouble(params.at(4));
				const bool originalState = pao->pause;

				pao->pause = true;
				StopWatch::StaticReset();
				AudioProcessor::Flanger(pao->buffer, depth, -0.37, 0.3, delay, SineWaveOscillator(1.0, rate, pao->buffer.FormatInfo().sampleRate, phase, AngleUnit::Degree));
				PrintDeltaTime("flanger applied in");
				pao->pause = originalState;
			}
			else if (sb.Contains("filter"))
			{
				std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
				std::vector<StringBuffer> params = sb.Split(' ');
				heph_float f1 = StringBuffer::StringToDouble(params.at(2));
				heph_float f2 = params.size() > 3 ? StringBuffer::StringToDouble(params.at(3)) : 0.0;

				if (params.at(1) == L"lp")
				{
					StopWatch::StaticReset();
					AudioProcessor::LowPassFilterMT(pao->buffer, 512, 1024, f1);
					PrintDeltaTime("low-pass filter applied in");
				}
				else if (params.at(1) == L"hp")
				{
					StopWatch::StaticReset();
					AudioProcessor::HighPassFilterMT(pao->buffer, 512, 1024, f1);
					PrintDeltaTime("high-pass filter applied in");
				}
				else if (params.at(1) == L"bp")
				{
					StopWatch::StaticReset();
					AudioProcessor::BandPassFilterMT(pao->buffer, 512, 1024, f1, f2);
					PrintDeltaTime("band-pass filter applied in");
				}
				else if (params.at(1) == L"bc")
				{
					StopWatch::StaticReset();
					AudioProcessor::BandCutFilterMT(pao->buffer, 512, 1024, f1, f2);
					PrintDeltaTime("band-cut filter applied in");
				}
			}
			else if (sb.Contains("speed"))
			{
				std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
				std::vector<StringBuffer> params = sb.Split(' ');
				const bool originalState = pao->pause;
				const heph_float originalPosition = audio.GetAOPosition(pao);
				heph_float speed = StringBuffer::StringToDouble(params.at(1));
				pao->pause = true;
				StopWatch::StaticReset();
				AudioProcessor::ChangeSpeedTD(pao->buffer, speed);
				audio.SetAOPosition(pao, originalPosition);
				PrintDeltaTime("playback speed changed in");
				pao->pause = originalState;
			}
			else if (sb.Contains("pitch"))
			{
				std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
				std::vector<StringBuffer> params = sb.Split(' ');
				const bool originalState = pao->pause;
				heph_float shiftFactor = StringBuffer::StringToDouble(params.at(1));
				pao->pause = true;
				StopWatch::StaticReset();
				AudioProcessor::PitchShiftMT(pao->buffer, 1024, 4096, shiftFactor);
				PrintDeltaTime("pitch shifted in");
				pao->pause = originalState;
			}
			else if (sb == L"original")
			{
				StopWatch::StaticReset();
				std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
				const bool originalState = pao->pause;
				const heph_float originalPosition = audio.GetAOPosition(pao);
				pao->pause = true;
				File audioFile = File(pao->filePath, FileOpenMode::Read);
				FileFormats::IAudioFileFormat* audioFormat = FileFormats::AudioFileFormatManager::FindFileFormat(pao->filePath);
				pao->buffer = audioFormat->ReadFile(&audioFile);
				pao->buffer.SetChannelCount(audio.GetRenderFormat().channelCount);
				pao->buffer.SetSampleRate(audio.GetRenderFormat().sampleRate);
				audio.SetAOPosition(pao, originalPosition);
				PrintDeltaTime("removed all effects in");
				pao->pause = originalState;
			}
		}
		catch (...)
		{
			ConsoleLogger::Log("Invalid input!", ConsoleLogger::error);
		}
	}

	audio.StopRendering();

	return 0;
}