#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
void SetToDefaultDevice(AudioDevice device);

Audio* audio;
int main()
{
	audio = new Audio();
	// C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav
	audio->SetOnExceptionHandler(OnException);
	audio->SetOnDefaultAudioDeviceChangeHandler(SetToDefaultDevice);
	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	std::shared_ptr<IAudioObject> pao = audio->Play(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav", 0u, true);
	AudioProcessor::LowPassFilter(pao->buffer, 200.0, 20.0);
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