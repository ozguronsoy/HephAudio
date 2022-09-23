#include <iostream>
#include <string>
#include <INativeAudio.h>
#include <WinAudio.h>
#include <AudioProcessor.h>
#include <Fourier.h>

using namespace HephAudio;
using namespace HephAudio::Structs;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
void SetToDefaultDevice(AudioDevice device);

WinAudio* wa;
int main()
{
	wa = new WinAudio();
	// C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav
	wa->OnException = OnException;
	wa->OnAudioDeviceAdded = SetToDefaultDevice;
	wa->OnAudioDeviceRemoved = SetToDefaultDevice;
	wa->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	std::shared_ptr<IAudioObject> pao = wa->Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav");
	pao->loopCount = 0u;
	pao->paused = false;

	std::string a;
	std::cin >> a;
	delete wa;
	std::cin >> a;
	return 0;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::wcout << std::endl << std::endl << ex.WhatW() << std::endl << std::endl;
}
void SetToDefaultDevice(AudioDevice device) 
{
	std::cout << "Is current device: " << (device.id == wa->GetRenderDevice().id) << "\n";
	wa->StopRendering();
	wa->InitializeRender(nullptr, wa->GetRenderFormat());
}