# Using Windows

Windows are used for reducing the artifacts introduced by the ***Fourier transform***. Or to isolate the signal over some interval. 
Windowing is applied by performing point-by-point multiplication with the selected window function and the signal. 
In this tutorial, i will show an example of how we can utilize the window classes. <br>
Lets take a look at the ``AudioProcessor::HighPassFilter`` method. It takes 3 parameters; the audio data (buffer), a cutoff frequency, and a reference to a window instance. 
We can select any window we want. If you are unsure about which window to use, you can always select Hann window. Then try and compare other windows until you find the one that satisfies your needs. 

```c++
HannWindow hannWindow;
AudioProcessor::HighPassFilter(pAudioObject->buffer, 100.0, hannWindow);
```