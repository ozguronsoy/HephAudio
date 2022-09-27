#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>

using namespace HephAudio;
using namespace HephAudio::Structs;
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
	std::shared_ptr<IAudioObject> pao = audio->Play(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav", 0u, true);
	pao->paused = false;

	AudioBuffer b1 = AudioBuffer(5, AudioFormatInfo(1, 2, 32, 48000));
	b1.Set(0.1, 0, 0);
	b1.Set(0.2, 0, 1);
	b1.Set(0.3, 1, 0);
	b1.Set(0.4, 1, 1);
	b1.Set(0.5, 2, 0);
	b1.Set(0.6, 2, 1);
	b1.Set(0.7, 3, 0);
	b1.Set(0.8, 3, 1);
	b1.Set(0.9, 4, 0);
	b1.Set(1.0, 4, 1);
	AudioBuffer b2 = AudioBuffer(3, AudioFormatInfo(1, 2, 32, 48000));
	b2.Set(1.0, 0, 0);
	b2.Set(0.9, 0, 1);
	b2.Set(0.8, 1, 0);
	b2.Set(0.7, 1, 1);
	b2.Set(0.6, 2, 0);
	b2.Set(0.5, 2, 1);

	for (size_t i = 0; i < b1.FrameCount(); i++)
	{
		for (size_t j = 0; j < b1.GetFormat().channelCount; j++)
		{
			std::cout << b1.Get(i, j) << "\n";
		}
	}

	std::cout << "\n\n";
	b1.Replace(b2, 4, 2);
	for (size_t i = 0; i < b1.FrameCount(); i++)
	{
		for (size_t j = 0; j < b1.GetFormat().channelCount; j++)
		{
			std::cout << b1.Get(i, j) << "\n";
		}
	}

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