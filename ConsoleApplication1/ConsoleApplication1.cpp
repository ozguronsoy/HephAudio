#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
void SetToDefaultDevice(AudioDevice device);
void OnRender(IAudioObject* sender, AudioBuffer& renderBuffer, size_t frameIndex);
double PrintDeltaTime(const char* label);

Audio* audio;
int main()
{
	audio = new Audio();
	// C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav
	audio->SetOnExceptionHandler(OnException);
	audio->SetOnDefaultAudioDeviceChangeHandler(SetToDefaultDevice);

	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	PrintDeltaTime("Init Render");

	std::shared_ptr<IAudioObject> pao = audio->Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav");
	pao->OnRender = OnRender;
	pao->loopCount = 0u;
	PrintDeltaTime("Load File");

	AudioProcessor::LowPassFilter(pao->buffer, 8192, 8192*2, 650.0, 0.0);
	PrintDeltaTime("Filter");

	pao->paused = false;

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
void OnRender(IAudioObject* sender, AudioBuffer& renderBuffer, size_t frameIndex)
{
	//PrintDeltaTime("");
	//AudioProcessor::LowPassFilterRT(sender->buffer, renderBuffer, frameIndex, 650.0, 0.0);
	//PrintDeltaTime("Filter");
}
double PrintDeltaTime(const char* label)
{
	static std::chrono::high_resolution_clock clock;
	static std::chrono::steady_clock::time_point t1 = clock.now();
	static std::chrono::steady_clock::time_point t2 = t1;
	static double dt = 0.0;
	t2 = clock.now();
	dt = (t2 - t1).count() * 0.000001;
	std::cout << label << ": " << dt << "ms\n";
	t1 = t2 = clock.now();
	return dt;
}