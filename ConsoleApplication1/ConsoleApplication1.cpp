#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>
#include <ConsoleLogger.h>
#include <StopWatch.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnDeviceAdded(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnDeviceRemoved(AudioEventArgs* pArgs, AudioEventResult* pResult);
void OnRender(AudioEventArgs* pArgs, AudioEventResult* pResult);
void RenderBlackened(AudioEventArgs* pArgs, AudioEventResult* pResult);
double PrintDeltaTime(StringBuffer label);
double FVM(double f) { return 0.0; }

constexpr const char* queueName = "My Queue";
int main()
{
	Audio audio = Audio();
	audio.SetOnExceptionHandler(OnException);
	audio.SetOnAudioDeviceAddedHandler(OnDeviceAdded);
	audio.SetOnAudioDeviceRemovedHandler(OnDeviceRemoved);

	AudioDevice drd = audio.GetDefaultAudioDevice(AudioDeviceType::Render);
	audio.InitializeRender(&drd, AudioFormatInfo(1, 2, 32, 48000));

	std::vector<StringBuffer> filesToQueue = {
		"C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav", "C:\\Users\\ozgur\\Desktop\\AudioFiles\\deneme.wav", "C:\\Users\\ozgur\\Desktop\\AudioFiles\\blackened.wav",
		"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav", "C:\\Users\\ozgur\\Desktop\\AudioFiles\\Fatima.wav",
		"C:\\Users\\ozgur\\Desktop\\AudioFiles\\asdf.wav", "C:\\Users\\ozgur\\Desktop\\AudioFiles\\deneme2.wav"
	};
	std::vector<std::shared_ptr<AudioObject>> paos = audio.Queue(queueName, 250.0, filesToQueue);
	audio.RegisterCategory(Category(queueName, 1.0));

	for (size_t i = 0; i < paos.size(); i++)
	{
		paos.at(i)->categories.push_back(queueName);
		if (paos.at(i)->name == "blackened.wav")
		{
			paos.at(i)->OnRender = RenderBlackened;
		}
		else
		{
			paos.at(i)->OnRender = OnRender;
		}
	}
	filesToQueue.clear();
	paos.clear();

	std::string a;
	StringBuffer sb = "";
	while (sb != "exit")
	{
		std::getline(std::cin, a);
		sb = a.c_str();
		sb.ToLower();
		if (sb.Contains("skip"))
		{
			uint32_t skipCount = StringBuffer::StringToUI32(sb.SubString(sb.Find(' '), 10));
			audio.Skip(skipCount, queueName, false);
		}
		else if (sb.Contains("volume"))
		{
			double volume = StringBuffer::StringToDouble(sb.SubString(sb.Find(' '), 10));
			audio.SetCategoryVolume(queueName, volume);
		}
		else if (sb.Contains("position"))
		{
			double position = StringBuffer::StringToDouble(sb.SubString(sb.Find(' '), 10));
			audio.SetAOPosition(audio.GetAO(queueName, 0), position);
		}
		else if (sb == "pause")
		{
			audio.GetAO(queueName, 0)->pause = true;
		}
		else if (sb == "resume")
		{
			audio.GetAO(queueName, 0)->pause = false;
		}
	}

	return 0;
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

	const size_t readFrameCount = (double)pRenderArgs->renderFrameCount * pAudioObject->buffer.FormatInfo().sampleRate / pNativeAudio->GetRenderFormat().sampleRate;

	pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, readFrameCount);

	AudioProcessor::ConvertSampleRate(pRenderResult->renderBuffer, pNativeAudio->GetRenderFormat().sampleRate, pRenderArgs->renderFrameCount);

	pAudioObject->frameIndex += readFrameCount;
	pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
}
void RenderBlackened(AudioEventArgs* pArgs, AudioEventResult* pResult)
{
	NativeAudio* pNativeAudio = (NativeAudio*)pArgs->pNativeAudio;
	AudioObject* pAudioObject = (AudioObject*)pArgs->pAudioObject;
	AudioRenderEventArgs* pRenderArgs = (AudioRenderEventArgs*)pArgs;
	AudioRenderEventResult* pRenderResult = (AudioRenderEventResult*)pResult;

	const size_t readFrameCount = (double)pRenderArgs->renderFrameCount * pAudioObject->buffer.FormatInfo().sampleRate / pNativeAudio->GetRenderFormat().sampleRate;

	pRenderResult->renderBuffer = pAudioObject->buffer.GetSubBuffer(pAudioObject->frameIndex, readFrameCount);

	AudioProcessor::ReverseRT(pAudioObject->buffer, pRenderResult->renderBuffer, pAudioObject->frameIndex);
	AudioProcessor::ConvertSampleRate(pRenderResult->renderBuffer, pNativeAudio->GetRenderFormat().sampleRate, pRenderArgs->renderFrameCount);

	pAudioObject->frameIndex += readFrameCount;
	pRenderResult->isFinishedPlaying = pAudioObject->frameIndex >= pAudioObject->buffer.FrameCount();
}
double PrintDeltaTime(StringBuffer label)
{
	const double dt = StopWatch::DeltaTime(StopWatch::milli);
	label = label + " " + StringBuffer::ToString(dt, 4);
	label += " ms";
	ConsoleLogger::LogLine(label, ConsoleLogger::debug);
	StopWatch::Reset();
	return dt;
}