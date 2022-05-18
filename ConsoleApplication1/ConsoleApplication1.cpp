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
void OnRender(IAudioObject* sender, AudioBuffer& buffer, size_t frameIndex);
bool IsFinishedPlaying(IAudioObject* sender);

EchoInfo echoInfo;

int main()
{
	// C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav
	WinAudio wa;
	wa.OnException = &OnException;
	wa.InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	echoInfo.reflectionCount = 5;
	echoInfo.reflectionDelay = 3.5; // in seconds
	echoInfo.volumeFactor = 0.25;
	std::shared_ptr<IAudioObject> pAudioObject = wa.Play(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav");
	pAudioObject->IsFinishedPlaying = IsFinishedPlaying;
	pAudioObject->OnRender = OnRender;

	std::string a;
	std::cin >> a;
	return 0;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::wcout << std::endl << std::endl << ex.WhatW() << std::endl << std::endl;
}
void OnRender(IAudioObject* sender, AudioBuffer& buffer, size_t frameIndex)
{
	AudioProcessor::EchoSubBuffer(sender->buffer, buffer, frameIndex, echoInfo);
}
bool IsFinishedPlaying(IAudioObject* sender) 
{
	return sender->frameIndex >= echoInfo.CalculateAudioBufferFrameCount(sender->buffer);
}