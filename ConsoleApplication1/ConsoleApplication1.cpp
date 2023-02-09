#include <iostream>
#include <string>
#include <Audio.h>
#include <AudioProcessor.h>
#include <Fourier.h>
#include <ConsoleLogger.h>
#include <StopWatch.h>

using namespace HephAudio;
using namespace HephAudio::Native;

void OnException(AudioException ex, AudioExceptionThread t);
void OnRender(AudioObject* sender, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t renderFrameCount);
double PrintDeltaTime(StringBuffer label);
double FVM(double f) { return 0.0; }

Audio* audio;
int main()
{
	audio = new Audio();
	audio->SetOnExceptionHandler(OnException);

	AudioDevice drd = audio->GetDefaultAudioDevice(AudioDeviceType::Render);
	audio->InitializeRender(&drd, AudioFormatInfo(1, 2, 32, 48000));
	audio->InitializeCapture(nullptr, AudioFormatInfo(1, 2, 32, 48000));

	std::vector<StringBuffer> queue = {
		"C:\\Users\\ozgur\\Desktop\\AudioFiles\\piano2.wav", "C:\\Users\\ozgur\\Desktop\\AudioFiles\\deneme.wav", 
		"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav", "C:\\Users\\ozgur\\Desktop\\AudioFiles\\Fatima.wav", 
		"C:\\Users\\ozgur\\Desktop\\AudioFiles\\asdf.wav", "C:\\Users\\ozgur\\Desktop\\AudioFiles\\deneme2.wav" 
	};
	audio->Queue("My Queue", 250.0, queue);

	std::string a;
	std::cin >> a;
	audio->Skip(3, "My Queue", false);

	std::cin >> a;
	delete audio;
	std::cin >> a;

	return 0;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::string str = ("[" + AudioExceptionThreadName(t) + "] " + (char*)ex).fc_str();
	size_t pos = str.find('\n', 0);
	str.insert(pos + 1, 21, ' ');
	pos = str.find('\n', pos + 1);
	str.insert(pos + 1, 21, ' ');
	//ConsoleLogger::Log(str.c_str(), ConsoleLogger::error);
}
void OnRender(AudioObject* sender, AudioBuffer& subBuffer, size_t subBufferFrameIndex, size_t renderFrameCount)
{
}
double PrintDeltaTime(StringBuffer label)
{
	const double dt = StopWatch::DeltaTime(StopWatch::milli);
	label = label + " " + StringBuffer::ToString(dt, 4);
	label += " ms";
	ConsoleLogger::LogLine(label, ConsoleLogger::info);
	StopWatch::Reset();
	return dt;
}