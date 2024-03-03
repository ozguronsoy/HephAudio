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
You can take both of these parameters from the user, but for simplicity, we will use constant values in this tutorial.
```c++
// TODO 
```
