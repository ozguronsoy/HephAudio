# Using Windows

[Windows](/docs/HephAudio/Windows) are used for reducing the artifacts introduced by the ***Fourier transform***. Or to isolate the signal over some interval. 
Windowing is applied by performing point-by-point multiplication with the selected window function and the signal. 
In this tutorial, i will show an example of how we can utilize the window classes. <br>
Lets take a look at the ``AudioProcessor::HighPassFilter`` method. It takes 3 parameters; the audio data (buffer), a cutoff frequency, and a reference to a window instance. 
We can select any window we want. If you are unsure about which window to use, you can always select Hann window. Then try and compare other windows until you find the one that satisfies your needs. 
```c++
HannWindow hannWindow;
AudioProcessor::HighPassFilter(pAudioObject->buffer, 100.0, hannWindow);
```
Passing some parameter to a function is easy. Let's write our own high-pass filter to better understand how we can utilize the window classes while creating our own sound effects and filters.

### Custom High-Pass Filter

A high-pass filter eliminates the frequencies that are lower than the provided threshold. 
To achieve this we have to split the signal into its frequency components, eliminate the aforementioned frequencies, and then reconstruct the filtered signal. 
```c++
void MyHighPassFilter(AudioBuffer& buffer, heph_float cutoffFrequency, Window& window);
```
For performance and sound quality reasons, we are not going to apply the filter to the whole buffer at once. 
Instead, we are going to apply it over an interval. Let's call the width of that interval in terms of frames, the ``fftSize``. 
Then we are going to shift that interval by a constant value, ``hopSize``, and apply the filter again. We are going to repeat this process until the whole buffer is filtered. 

> [!NOTE]
> The methods that use FFT take too much time on ***DEBUG*** mode, so use the ***RELEASE*** mode to test this function (enable the maximum optimization for speed).

```c++
void MyHighPassFilter(AudioBuffer& buffer, heph_float cutoffFrequency, Window& window)
{
	constexpr size_t hopSize = 512;
	constexpr size_t fftSize = 2048; // must be a power of 2

	const size_t frameCount = buffer.FrameCount();
	const size_t nyquistFrequency = fftSize * 0.5;

	// calculate the bin index that corresponds to the cutoff frequency
	const uint64_t stopIndex = Fourier::BinFrequencyToIndex(buffer.FormatInfo().sampleRate, fftSize, cutoffFrequency);

	window.SetSize(fftSize);
	const FloatBuffer windowBuffer = window.GenerateBuffer(); // so we don't have to calculate the window more than once

	
	std::vector<FloatBuffer> channels = AudioProcessor::SplitChannels(buffer);
	buffer.Reset(); // set all samples to 0

	for (size_t i = 0; i < frameCount; i += hopSize)
	{
		for (size_t j = 0; j < channels.size(); j++)
		{
			// Get the part of the signal we want to process and apply windowing
			const FloatBuffer windowedAudioSignal = channels[j].GetSubBuffer(i, fftSize) * windowBuffer;

			// Split the windowed audio data into its frequency components
			ComplexBuffer frequencyComponents = Fourier::FFT_Forward(windowedAudioSignal, fftSize);

			// remove the frequencies lower than the cutoff frequency
			for (size_t k = 0; k < stopIndex && k < nyquistFrequency; k++)
			{
				frequencyComponents[k].real = 0;
				frequencyComponents[k].imaginary = 0;
				frequencyComponents[fftSize - k - 1] = frequencyComponents[k].Conjugate();
			}

			// take the IFFT but do not divide the samples by fftSize (scale)
			// so we don't use an extra loop
			Fourier::FFT_Inverse(frequencyComponents, false);

			// apply window again and scale
			for (size_t k = 0, l = i; k < fftSize && l < frameCount; k++, l++)
			{
				buffer[l][j] += frequencyComponents[k].real * windowBuffer[k] / fftSize;
			}
		}
	}
}
```
