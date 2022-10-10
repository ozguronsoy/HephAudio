#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
void SetToDefaultDevice(AudioDevice device);
inline void PrintDeltaTime();

Audio* audio;
int main()
{
	audio = new Audio();
	// C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav
	audio->SetOnExceptionHandler(OnException);
	audio->SetOnDefaultAudioDeviceChangeHandler(SetToDefaultDevice);

	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	PrintDeltaTime();

	std::shared_ptr<IAudioObject> pao = audio->Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav");
	pao->loopCount = 0u;
	PrintDeltaTime();

	AudioProcessor::Reverse(pao->buffer);
	PrintDeltaTime(); // ~0.09ms in Release

	AudioProcessor::LowPassFilter(pao->buffer, 200.0, 20.0);
	PrintDeltaTime(); // ~930ms in Release

	EchoInfo ei = EchoInfo();
	ei.echoStartPosition = 0.0;
	ei.echoEndPosition = 1.0;
	ei.reflectionCount = 10;
	ei.reflectionDelay = pao->buffer.CalculateDuration() * 0.5;
	ei.volumeFactor = 0.5;
	AudioProcessor::Echo(pao->buffer, ei);
	PrintDeltaTime(); // ~500ms in release

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
inline void PrintDeltaTime()
{
	static std::chrono::high_resolution_clock clock;
	static std::chrono::steady_clock::time_point t1 = clock.now();
	static std::chrono::steady_clock::time_point t2 = t1;
	static unsigned short cc = 0u;
	static double dt = 0.0;
	t2 = clock.now();
	dt = (t2 - t1).count() * 0.000001;
	std::cout << cc << ": " << dt << "ms\n";
	t1 = t2 = clock.now();
	cc++;
}