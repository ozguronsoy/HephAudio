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

	int16_t s = INT16_MIN;
	uint16_t us = UINT16_MAX;

	int32_t i = INT32_MIN;
	uint32_t ui = UINT32_MAX;

	int64_t l = INT64_MIN;
	uint64_t ul = UINT64_MAX;

	StringBuffer ss = StringBuffer::ToHexString(s);
	StringBuffer uss = StringBuffer::ToHexString(us);
	StringBuffer is = StringBuffer::ToHexString(i);
	StringBuffer uis = StringBuffer::ToHexString(ui);
	StringBuffer ls = StringBuffer::ToHexString(l);
	StringBuffer uls = StringBuffer::ToHexString(ul);

	printf("\n\n");
	ConsoleLogger::LogLine("short: " + ss, ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned short: " + uss, ConsoleLogger::debug);
	ConsoleLogger::LogLine("int: " + is, ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned int: " + uis, ConsoleLogger::debug);
	ConsoleLogger::LogLine("long long: " + ls, ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned long long: " + uls, ConsoleLogger::debug);

	s = us = i = ui = l = ul = 0;

	s = StringBuffer::HexStringToI16(ss);
	us = StringBuffer::HexStringToUI16(uss);
	i = StringBuffer::HexStringToI32(is);
	ui = StringBuffer::HexStringToUI32(uis);
	l = StringBuffer::HexStringToI64(ls);
	ul = StringBuffer::HexStringToUI64(uls);

	printf("\n\n");
	ConsoleLogger::LogLine("short: " + StringBuffer::ToString(s), ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned short: " + StringBuffer::ToString(us), ConsoleLogger::debug);
	ConsoleLogger::LogLine("int: " + StringBuffer::ToString(i), ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned int: " + StringBuffer::ToString(ui), ConsoleLogger::debug);
	ConsoleLogger::LogLine("long long: " + StringBuffer::ToString(l), ConsoleLogger::debug);
	ConsoleLogger::LogLine("unsigned long long: " + StringBuffer::ToString(ul), ConsoleLogger::debug);
	printf("\n\n");

	std::vector<AudioDevice> audioDevices = audio->GetAudioDevices(AudioDeviceType::All, true);
	for (size_t i = 0; i < audioDevices.size(); i++)
	{
		ConsoleLogger::LogLine("(" + audioDevices.at(i).id + ") " + audioDevices.at(i).name, ConsoleLogger::debug);
	}

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
	std::string str = ("[" + AudioExceptionThreadName(t) + "] " + (wchar_t*)ex).fc_str();
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