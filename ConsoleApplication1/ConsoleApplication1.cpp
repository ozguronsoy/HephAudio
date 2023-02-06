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
	StopWatch::Start();
	audio = new Audio();
	audio->SetOnExceptionHandler(OnException);

	StopWatch::Reset();
	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	PrintDeltaTime("render initialized in");

	//std::shared_ptr<AudioObject> pao = audio->Load("C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav");
	//pao->OnRender = OnRender;
	//pao->loopCount = 1u;
	//PrintDeltaTime("file loaded in");

	//AudioProcessor::HighPassFilter(pao->buffer, 512, 1024, 1000.0, FVM);
	//AudioProcessor::HighPassFilterMT(pao->buffer, 512, 1024, 1000.0, FVM);
	//PrintDeltaTime("filter");

	//pao->pause = false;

	short s = INT16_MIN;
	unsigned short us = UINT16_MAX;

	int i = INT32_MIN;
	unsigned int ui = UINT32_MAX;

	long long l = INT64_MIN;
	unsigned long long ul = UINT64_MAX;

	ConsoleLogger::LogLine("short: " + StringBuffer::ToString(s), ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned short: " + StringBuffer::ToString(us), ConsoleLogger::debug);
	ConsoleLogger::LogLine("int: " + StringBuffer::ToString(i), ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned int: " + StringBuffer::ToString(ui), ConsoleLogger::debug);
	ConsoleLogger::LogLine("long long: " + StringBuffer::ToString(l), ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned long long: " + StringBuffer::ToString(ul), ConsoleLogger::debug);


	std::string a;
	std::cin >> a;
	delete audio;
	//pao = nullptr;
	StopWatch::Stop();
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
	ConsoleLogger::Log(str.c_str(), ConsoleLogger::error);
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