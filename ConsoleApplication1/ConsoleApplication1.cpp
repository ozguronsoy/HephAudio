#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>
#include <ConsoleLogger.h>
#include <StopWatch.h>
#include "FloatBuffer.h"
#include <shlobj.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnDeviceAdded(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnDeviceRemoved(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnRender(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnFinishedPlaying(AudioEventArgs* pArgs, AudioEventResult* pResult);
hephaudio_float PrintDeltaTime(StringBuffer label);
int Run(Audio& audio, StringBuffer& audioRoot);
void HPFMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFrequency, size_t threadCountPerChannel);

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
	
	Audio audio = Audio();
	audio->OnException = OnException;
	audio->OnAudioDeviceAdded = OnDeviceAdded;
	audio->OnAudioDeviceRemoved = OnDeviceRemoved;
	audio->SetDeviceEnumerationPeriod(250e6);

	audio.InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48e3));

	//AudioFile readFile = AudioFile(audioRoot + "Gate of Steiner.wav", AudioFileOpenMode::Read);
	//Formats::IAudioFormat* pAudioFormat = audio.GetAudioFormats()->GetAudioFormat(readFile.FilePath());
	//AudioBuffer buffer = pAudioFormat->ReadFile(&readFile);

	//buffer.Cut(0, buffer.FrameCount() * 0.44);

	//AudioProcessor::PitchShift(buffer, 1.11111111111);
	//AudioFormatInfo formatInfo = buffer.FormatInfo();
	//formatInfo.formatTag = WAVE_FORMAT_IEEE_FLOAT;
	//buffer.SetFormat(formatInfo);
	////AudioProcessor::ConvertInnerToPcmFormat(buffer, 16, Endian::Little);

	//StopWatch::Reset();
	//pAudioFormat->SaveToFile(desktopPath + (StringBuffer)"\\deneme.wav", buffer, true);
	//PrintDeltaTime("save completed in");

	StopWatch::Reset();
	auto pao = audio.Load(desktopPath + (StringBuffer)"\\deneme.wav");
	PrintDeltaTime("dt");

	if (pao != nullptr)
	{
		pao->loopCount = 0;
		pao->OnFinishedPlaying = OnFinishedPlaying;
		pao->pause = false;
	}

	return Run(audio, audioRoot);
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
	ConsoleLogger::Log(str.c_str(), ConsoleLogger::error);
}
void OnDeviceAdded(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	NativeAudio* pNativeAudio = (NativeAudio*)pArgs->pNativeAudio;
	AudioDeviceEventArgs* pDeviceArgs = (AudioDeviceEventArgs*)pArgs;

	if (pDeviceArgs->audioDevice.type == AudioDeviceType::Render && pDeviceArgs->audioDevice.isDefault)
	{
		pNativeAudio->StopRendering();
		pNativeAudio->InitializeRender(&pDeviceArgs->audioDevice, pNativeAudio->GetRenderFormat());
	}
}
void OnDeviceRemoved(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	NativeAudio* pNativeAudio = (NativeAudio*)pArgs->pNativeAudio;
	AudioDeviceEventArgs* pDeviceArgs = (AudioDeviceEventArgs*)pArgs;

	if (pDeviceArgs->audioDevice.id == pNativeAudio->GetRenderDevice().id)
	{
		pNativeAudio->StopRendering();
		pNativeAudio->InitializeRender(nullptr, pNativeAudio->GetRenderFormat());
	}
}
void OnRender(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	NativeAudio* pNativeAudio = (NativeAudio*)pArgs->pNativeAudio;
	AudioObject* pAudioObject = (AudioObject*)pArgs->pAudioObject;
	AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
	AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

	const size_t readFrameCount = (hephaudio_float)pRenderArgs->renderFrameCount * (hephaudio_float)pAudioObject->buffer.FormatInfo().sampleRate / (hephaudio_float)pNativeAudio->GetRenderFormat().sampleRate;

	pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, readFrameCount);

	AudioProcessor::ConvertSampleRateRT(pAudioObject->buffer, pRenderResult->renderBuffer, pAudioObject->frameIndex, pNativeAudio->GetRenderFormat().sampleRate, pRenderArgs->renderFrameCount);

	pAudioObject->frameIndex += readFrameCount;
	pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
}
void OnFinishedPlaying(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	AudioObject* pAudioObject = (AudioObject*)pArgs->pAudioObject;
	AudioFinishedPlayingEventArgs* pFinishedPlayingArgs = (AudioFinishedPlayingEventArgs*)pArgs;

	if (pFinishedPlayingArgs->remainingLoopCount > 0)
	{
		ConsoleLogger::Log("The track \"" + pAudioObject->name + "\" is starting over, number of remaning loops: " + StringBuffer::ToString(pFinishedPlayingArgs->remainingLoopCount), ConsoleLogger::debug);
	}
}
hephaudio_float PrintDeltaTime(StringBuffer label)
{
	const hephaudio_float dt = StopWatch::DeltaTime(StopWatch::milli);
	label = label + " " + StringBuffer::ToString(dt, 4);
	label += " ms";
	ConsoleLogger::Log(label, ConsoleLogger::success);
	StopWatch::Reset();
	return dt;
}
int Run(Audio& audio, StringBuffer& audioRoot)
{
	std::wstring a;
	StringBuffer sb = "";
	while (sb != L"exit" && sb != L"quit")
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
			std::shared_ptr<AudioObject> pao = audio.Play(audioRoot + sb.Split('\"').at(1), isNumber ? StringBuffer::HexStringToUI32(arg1) : 1);
			pao->OnRender = OnRender;
			pao->OnFinishedPlaying = OnFinishedPlaying;
		}
		else if (sb.Contains("load"))
		{
			StopWatch::Reset();
			std::shared_ptr<AudioObject> pao = audio.Load(audioRoot + sb.Split('\"').at(1));
			pao->OnFinishedPlaying = OnFinishedPlaying;
			PrintDeltaTime("File loaded in");
		}
		else if (sb.Contains("volume"))
		{
			hephaudio_float volume = StringBuffer::StringToDouble(sb.SubString(sb.Find(' '), 10));
			audio.GetAO("", 0)->volume = volume;
		}
		else if (sb.Contains("position"))
		{
			hephaudio_float position = StringBuffer::StringToDouble(sb.SubString(sb.Find(' '), 10));
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
			StopWatch::Reset();
			AudioProcessor::Reverse(audio.GetAO("", 0)->buffer);
			PrintDeltaTime("reverse applied in");
		}
		else if (sb == L"normalize")
		{
			StopWatch::Reset();
			AudioProcessor::Normalize(audio.GetAO("", 0)->buffer, 1.0);
			PrintDeltaTime("normalized in");
		}
		else if (sb.Contains("distortion"))
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			std::vector<StringBuffer> params = sb.Split(' ');
			hephaudio_float distortionParam = StringBuffer::StringToDouble(params.at(2));
			if (params.at(1) == L"hc")
			{
				StopWatch::Reset();
				AudioProcessor::HardClipDistortion(pao->buffer, distortionParam);
				PrintDeltaTime("hard-clipping distortion applied in");
			}
			else if (params.at(1) == L"sc")
			{
				StopWatch::Reset();
				AudioProcessor::SoftClipDistortion(pao->buffer, distortionParam);
				PrintDeltaTime("soft-clipping distortion applied in");
			}
			else if (params.at(1) == L"od")
			{
				StopWatch::Reset();
				AudioProcessor::Overdrive(pao->buffer, distortionParam);
				PrintDeltaTime("overdrive applied in");
			}
			else if (params.at(1) == L"fz")
			{
				hephaudio_float alpha = StringBuffer::StringToDouble(params.at(3));
				StopWatch::Reset();
				AudioProcessor::Fuzz(pao->buffer, distortionParam, alpha);
				PrintDeltaTime("fuzz applied in");
			}
		}
		else if (sb.Contains("flanger"))
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			std::vector<StringBuffer> params = sb.Split(' ');
			hephaudio_float depth = StringBuffer::StringToDouble(params.at(1));
			hephaudio_float delay = StringBuffer::StringToDouble(params.at(2));
			hephaudio_float rate = StringBuffer::StringToDouble(params.at(3));
			hephaudio_float phase = StringBuffer::StringToDouble(params.at(4));
			const bool originalState = pao->pause;

			pao->pause = true;
			StopWatch::Reset();
			AudioProcessor::Flanger(pao->buffer, depth, -0.37, 0.3, delay, SineWaveOscillator(1.0, rate, pao->buffer.FormatInfo().sampleRate, phase, AngleUnit::Degree));
			PrintDeltaTime("flanger applied in");
			pao->pause = originalState;
		}
		else if (sb.Contains("filter"))
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			std::vector<StringBuffer> params = sb.Split(' ');
			hephaudio_float f1 = StringBuffer::StringToDouble(params.at(2));
			hephaudio_float f2 = params.size() > 3 ? StringBuffer::StringToDouble(params.at(3)) : 0.0;

			if (params.at(1) == L"lp")
			{
				StopWatch::Reset();
				AudioProcessor::LowPassFilterMT(pao->buffer, 512, 1024, f1);
				PrintDeltaTime("low-pass filter applied in");
			}
			else if (params.at(1) == L"hp")
			{
				StopWatch::Reset();
				AudioProcessor::HighPassFilterMT(pao->buffer, 512, 1024, f1);
				PrintDeltaTime("high-pass filter applied in");
			}
			else if (params.at(1) == L"bp")
			{
				StopWatch::Reset();
				AudioProcessor::BandPassFilterMT(pao->buffer, 512, 1024, f1, f2);
				PrintDeltaTime("band-pass filter applied in");
			}
			else if (params.at(1) == L"bc")
			{
				StopWatch::Reset();
				AudioProcessor::BandCutFilterMT(pao->buffer, 512, 1024, f1, f2);
				PrintDeltaTime("band-cut filter applied in");
			}
		}
		else if (sb.Contains("speed"))
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			std::vector<StringBuffer> params = sb.Split(' ');
			const bool originalState = pao->pause;
			const hephaudio_float originalPosition = audio.GetAOPosition(pao);
			hephaudio_float speed = StringBuffer::StringToDouble(params.at(1));
			pao->pause = true;
			StopWatch::Reset();
			AudioProcessor::ChangeSpeedTD(pao->buffer, 1440, 2880, speed);
			audio.SetAOPosition(pao, originalPosition);
			PrintDeltaTime("playback speed changed in");
			pao->pause = originalState;
		}
		else if (sb.Contains("pitch"))
		{
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			std::vector<StringBuffer> params = sb.Split(' ');
			const bool originalState = pao->pause;
			hephaudio_float shiftFactor = StringBuffer::StringToDouble(params.at(1));
			pao->pause = true;
			StopWatch::Reset();
			AudioProcessor::PitchShift(pao->buffer, 1024, 4096, shiftFactor);
			PrintDeltaTime("pitch shifted in");
			pao->pause = originalState;
		}
		else if (sb == L"original")
		{
			StopWatch::Reset();
			std::shared_ptr<AudioObject> pao = audio.GetAO("", 0);
			const bool originalState = pao->pause;
			const hephaudio_float originalPosition = audio.GetAOPosition(pao);
			pao->pause = true;
			AudioFile audioFile = AudioFile(pao->filePath, AudioFileOpenMode::Read);
			Formats::IAudioFormat* audioFormat = audio.GetAudioFormats()->GetAudioFormat(pao->filePath);
			pao->buffer = audioFormat->ReadFile(&audioFile);
			pao->buffer.SetChannelCount(audio.GetRenderFormat().channelCount);
			pao->buffer.SetSampleRate(audio.GetRenderFormat().sampleRate);
			audio.SetAOPosition(pao, originalPosition);
			PrintDeltaTime("removed all effects in");
			pao->pause = originalState;
		}
	}

	audio.StopRendering();

	return 0;
}
void HPFMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, hephaudio_float cutoffFrequency, size_t threadCountPerChannel)
{
	constexpr auto applyFilter = [](AudioBuffer* buffer, const FloatBuffer* window, uint16_t channelIndex, size_t frameIndex, size_t frameCount, size_t hopSize, size_t fftSize, size_t stopIndex)
	{
		FloatBuffer channel = FloatBuffer(frameCount);
		for (size_t i = 0; i < frameCount && i + frameIndex < buffer->FrameCount(); i++)
		{
			channel[i] = (*buffer)[i + frameIndex][channelIndex];
			(*buffer)[i + frameIndex][channelIndex] = 0;
		}

		for (size_t i = 0; i < channel.FrameCount(); i += hopSize)
		{
			ComplexBuffer complexBuffer = Fourier::FFT_Forward(channel.GetSubBuffer(i, fftSize), fftSize);
			for (int64_t j = stopIndex; j >= 0; j--)
			{
				complexBuffer[j] = Complex();
				complexBuffer[fftSize - j - 1] = Complex();
			}

			Fourier::FFT_Inverse(complexBuffer, false);
			for (size_t j = 0, k = i + frameIndex; j < fftSize && k < buffer->FrameCount(); j++, k++)
			{
				(*buffer)[k][channelIndex] += complexBuffer[j].real * (*window)[j] / (hephaudio_float)fftSize;
			}
		}
	};

	const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.FormatInfo().sampleRate, fftSize, cutoffFrequency);
	const size_t frameCountPerThread = ceil((hephaudio_float)buffer.FrameCount() / (hephaudio_float)threadCountPerChannel);
	const FloatBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);
	std::vector<std::thread> threads = std::vector<std::thread>(buffer.FormatInfo().channelCount * threadCountPerChannel);

	for (size_t i = 0; i < threadCountPerChannel; i++)
	{
		for (size_t j = 0; j < buffer.FormatInfo().channelCount; j++)
		{
			threads[i * buffer.FormatInfo().channelCount + j] = std::thread(applyFilter, &buffer, &hannWindow, j, i * frameCountPerThread, frameCountPerThread, hopSize, fftSize, stopIndex);
		}
	}

	for (size_t i = 0; i < threadCountPerChannel * buffer.FormatInfo().channelCount; i++)
	{
		if (threads[i].joinable())
		{
			threads[i].join();
		}
	}
}