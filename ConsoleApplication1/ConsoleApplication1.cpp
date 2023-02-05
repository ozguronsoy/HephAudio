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
void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction);

Audio* audio;
int main()
{
	StopWatch::Start();
	audio = new Audio();
	audio->SetOnExceptionHandler(OnException);

	StopWatch::Reset();
	audio->InitializeRender(nullptr, AudioFormatInfo(1, 2, 32, 48000));
	PrintDeltaTime("render initialized in");

	std::shared_ptr<AudioObject> pao = audio->Load(L"C:\\Users\\ozgur\\Desktop\\AudioFiles\\Gate of Steiner.wav");
	pao->OnRender = OnRender;
	pao->loopCount = 1u;
	PrintDeltaTime("file loaded in");

	//AudioProcessor::HighPassFilter(pao->buffer, 512, 1024, 1000.0, FVM);
	HighPassFilterMT(pao->buffer, 512, 1024, 1000.0, FVM);
	PrintDeltaTime("filter applied in");

	pao->pause = false;

	std::string a;
	std::cin >> a;
	delete audio;
	pao = nullptr;
	StopWatch::Stop();
	std::cin >> a;

	return 0;
}
void OnException(AudioException ex, AudioExceptionThread t)
{
	std::string str = ex.ToString();
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
	const double dt = StopWatch::DeltaTime(StopWatch::micro);
	label += " ";
	std::stringstream dts;
	dts.precision(4);
	dts << std::fixed << dt;
	label += dts.str().c_str();
	label += " us";
	ConsoleLogger::LogLine(label, ConsoleLogger::info);
	StopWatch::Reset();
	return dt;
}
void HighPassFilterMT(AudioBuffer& buffer, size_t hopSize, size_t fftSize, double cutoffFreq, FilterVolumeFunction volumeFunction)
{
	constexpr auto applyFilter = [](AudioBuffer* buffer, AudioBuffer* window, size_t hopSize, size_t fftSize, size_t stopIndex, FilterVolumeFunction volumeFunction)
	{
		AudioBuffer tempBuffer = *buffer;
		buffer->Reset();
		for (size_t i = 0; i < buffer->FrameCount(); i += hopSize)
		{
			ComplexBuffer complexBuffer = Fourier::FFT_Forward(tempBuffer.GetSubBuffer(i, fftSize), fftSize);
			for (int64_t j = stopIndex; j >= 0; j--)
			{
				complexBuffer[j] *= volumeFunction(Fourier::IndexToFrequency(buffer->FormatInfo().sampleRate, fftSize, j));
				complexBuffer[fftSize - j - 1] = Complex(complexBuffer[j].real, -complexBuffer[j].imaginary);
			}
			Fourier::FFT_Inverse(complexBuffer, false);
			for (size_t j = 0, k = i; j < fftSize && k < buffer->FrameCount(); j++, k++)
			{
				(*buffer)[k][0] += complexBuffer[j].real * (*window)[j][0] / fftSize;
			}
		}
	};

	fftSize = Fourier::CalculateFFTSize(fftSize);
	const uint64_t stopIndex = Fourier::FrequencyToIndex(buffer.FormatInfo().sampleRate, fftSize, cutoffFreq);

	std::vector<AudioBuffer*> channels;
	std::vector<std::thread> threads;

	AudioBuffer hannWindow = AudioProcessor::GenerateHannWindow(fftSize);

	for (size_t i = 0; i < buffer.FormatInfo().channelCount; i++)
	{
		AudioBuffer* channel = new AudioBuffer(buffer.FrameCount(), AudioFormatInfo(buffer.FormatInfo().formatTag, 1, buffer.FormatInfo().bitsPerSample, buffer.FormatInfo().sampleRate));
		for (size_t j = 0; j < buffer.FrameCount(); j++)
		{
			(*channel)[j][0] = buffer[j][i];
		}
		channels.push_back(channel);
		threads.push_back(std::thread(applyFilter, channel, &hannWindow, hopSize, fftSize, stopIndex, volumeFunction));
	}

	for (size_t i = 0; i < threads.size(); i++)
	{
		if (threads.at(i).joinable())
		{
			threads.at(i).join();
		}

		for (size_t j = 0; j < buffer.FrameCount(); j++)
		{
			buffer[j][i] = (*channels.at(i))[j][0];
		}

		delete channels.at(i);
	}
}