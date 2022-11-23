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
double PrintDeltaTime(const char* label);

Audio* audio;
std::vector<EqualizerInfo> infos = { EqualizerInfo(0.0, 250.0, [](double f) { return 0.0; }) };
int main()
{
	audio = new Audio();
	audio->SetOnExceptionHandler(OnException);
	audio->SetOnDefaultAudioDeviceChangeHandler(SetToDefaultDevice);

	PrintDeltaTime(nullptr);
	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	PrintDeltaTime("Init Render");

	std::shared_ptr<IAudioObject> pao = audio->Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav");
	pao->OnRender = OnRender;
	pao->loopCount = 0u;
	PrintDeltaTime("Load File");

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
	AudioProcessor::LowPassFilterRT(sender->buffer, subBuffer, subBufferFrameIndex, 650.0, [](double f) -> double { return 0.0; });
	PrintDeltaTime("Filter");
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