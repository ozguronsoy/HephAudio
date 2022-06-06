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
	wa.InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 96000));
	std::shared_ptr<IAudioObject> pao = wa.Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav");
	//AudioProcessor::LowPassFilter(pao->buffer, 800.0, 100.0);
	pao->loopCount = 0u;
	pao->paused = false;

	std::string a;
	std::cin >> a;
	return 0;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::wcout << std::endl << std::endl << ex.WhatW() << std::endl << std::endl;
}