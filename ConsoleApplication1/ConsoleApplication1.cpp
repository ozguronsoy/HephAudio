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

int main()
{
	// C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav
	WinAudio wa;
	wa.OnException = &OnException;
	wa.InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	std::shared_ptr<IAudioObject> pao = wa.Play(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav");

	std::string a;
	std::cin >> a;
	return 0;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::wcout << std::endl << std::endl << ex.WhatW() << std::endl << std::endl;
}