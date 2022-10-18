﻿#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
void SetToDefaultDevice(AudioDevice device);
inline void PrintDeltaTime(const char* label);

Audio* audio;
int main()
{
	audio = new Audio();
	// C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav
	audio->SetOnExceptionHandler(OnException);
	audio->SetOnDefaultAudioDeviceChangeHandler(SetToDefaultDevice);

	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 24, 48000));
	PrintDeltaTime("Init Render");

	std::shared_ptr<IAudioObject> pao = audio->Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav");
	pao->loopCount = 0u;
	PrintDeltaTime("Load File");

	/*AudioProcessor::Reverse(pao->buffer);
	PrintDeltaTime("Reverse");*/

	pao->buffer = -pao->buffer;
	PrintDeltaTime("operator -");

	pao->paused = false;

	std::string a;
	std::cin >> a;
	delete audio;
	//pao = nullptr;
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
inline void PrintDeltaTime(const char* label)
{
	static std::chrono::high_resolution_clock clock;
	static std::chrono::steady_clock::time_point t1 = clock.now();
	static std::chrono::steady_clock::time_point t2 = t1;
	static double dt = 0.0;
	t2 = clock.now();
	dt = (t2 - t1).count() * 0.000001;
	std::cout << label << ": " << dt << "ms\n";
	t1 = t2 = clock.now();
}