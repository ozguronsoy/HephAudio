# Device Enumeration

Available audio devices are updated periodically.

```cpp
// update every 3 seconds
audio.SetDeviceEnumerationPeriod(3000);
```

To get a list of available audio devices call the ``GetAudioDevices`` method.

```cpp
std::vector<AudioDevice> renderDevices = audio.GetAudioDevice(AudioDeviceType::Render);
std::vector<AudioDevice> captureDevices = audio.GetAudioDevices(AudioDeviceType::Capture);
std::vector<AudioDevice> allDevices = audio.GetAudioDevices(AudioDeviceType::All);
```

To get the default audio device, call the ``GetDefaultAudioDevice`` method.

```cpp
AudioDevice defaultRenderDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Render);
AudioDevice defaultCaptureDevice = audio.GetDefaultAudioDevice(AudioDeviceType::Capture);
```

After obtaining the audio devices, simply pass a pointer of the desired audio device to the ``InitializeRender`` method.

```cpp
audio.InitializeRender(&renderDevices[0], AudioFormatInfo(HEPHAUDIO_FORMAT_TAG_PCM, 16, HEPHAUDIO_CH_LAYOUT_STEREO, 48000));
```