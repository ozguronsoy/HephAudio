#include <iostream>
#include <string>
#include <thread>
#include <INativeAudio.h>
#include <WinAudio.h>

using namespace HephAudio;
using namespace HephAudio::Structs;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
int main()
{
	// C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav
	WinAudio wa;
	wa.OnException = &OnException;
	wa.InitializeRender(nullptr, AudioBuffer::CreateWaveFormat(WAVE_FORMAT_PCM, 2, 32, 48000));
	wa.Play(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav");

	auto start = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::high_resolution_clock::now() - start);
	std::cout << std::endl << duration.count() << " ns." << std::endl;

	std::string a;
	std::cin >> a;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::cout << std::endl << std::endl << ex.what() << std::endl << std::endl;
}