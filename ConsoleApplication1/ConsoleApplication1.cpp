#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
void SetToDefaultDevice(AudioDevice device);
void OnRender(IAudioObject* sender, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t renderFrameCount);
bool IsFinishedPlaying(IAudioObject* sender);
double PrintDeltaTime(const char* label);

Audio* audio;
EchoInfo ei;
int main()
{
	audio = new Audio();
	audio->SetOnExceptionHandler(OnException);
	audio->SetOnDefaultAudioDeviceChangeHandler(SetToDefaultDevice);

	PrintDeltaTime(nullptr);
	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	PrintDeltaTime("Init Render");

	std::shared_ptr<IAudioObject> pao = audio->Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav");
	pao->OnRender = OnRender;
	pao->IsFinishedPlaying = IsFinishedPlaying;
	pao->loopCount = 1u;
	PrintDeltaTime("Load File");

	ei.echoStartPosition = 0.0;
	ei.echoEndPosition = 1.0;
	ei.reflectionCount = 5;
	ei.reflectionDelay = pao->buffer.CalculateDuration() * 0.5;
	ei.volumeFactor = 0.5;

	//PrintDeltaTime(nullptr);
	//AudioProcessor::Echo(pao->buffer, ei);
	//PrintDeltaTime("Echo");

	pao->pause = false;

	std::string a;
	std::cin >> a;
	delete audio;
	pao = nullptr;
	std::cin >> a;
	return 0;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::wcout << std::endl << std::endl << ex.WhatW() << std::endl << std::endl;
}
void SetToDefaultDevice(AudioDevice device)
{
	std::cout << "Is current device: " << (device.id == audio->GetRenderDevice().id) << "\n";
	audio->InitializeRender(nullptr, audio->GetRenderFormat());
}
void OnRender(IAudioObject* sender, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t renderFrameCount)
{
	PrintDeltaTime(nullptr);
	AudioProcessor::EchoRT(sender->buffer, subBuffer, subBufferFrameIndex, ei);
	PrintDeltaTime("Echo");
}
bool IsFinishedPlaying(IAudioObject* sender)
{
	return sender->frameIndex >= ei.CalculateAudioBufferFrameCount(sender->buffer);
}
double PrintDeltaTime(const char* label)
{
	static std::chrono::high_resolution_clock clock;
	static std::chrono::steady_clock::time_point t1 = clock.now();
	static std::chrono::steady_clock::time_point t2 = t1;
	static double dt = 0.0;
	t2 = clock.now();
	dt = (t2 - t1).count() * 1.0e-6;
	if (label != nullptr)
	{
		std::cout << label << ": " << dt << "ms\n";
	}
	t1 = t2 = clock.now();
	return dt;
}