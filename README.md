# About
HephAudio is a cross-platform audio library that provides:
- Playing and recording audio data in Windows, Linux, iOS, MacOS, and Android.
- Audio device enumeration and selection.
- Tools for storing and processing audio data with ease.
- Easy to use sound effects and filters.
- FFT for frequency analysis of audio signals.
- Room impulse respnose simulation using source-image method.
- Reading and writing audio files.
- Decoding and encoding multiple audio codecs.

# Setup
### Visual Studio
1) Copy the HephAudio and HephCommon folders to your projects root folder.
2) Right click to your project, go to ``Configuration Properties -> C/C++ -> General -> Additional Including Directories`` and add the locations of the HephCommon and HephAudio header files
4) Now right click the solution and go to ``Add -> Existing Project``, under the HephCommon folder select ``HephCommon.vcxitems`` to add to your project. Repeat the same process for HephAudio.
5) Right click to your project, ``Add -> Reference -> Shared Projects`` and check both HephAudio and HephCommon.
6) Visual studio marks some of the standard functions as unsafe and prevents from compiling by throwing errors. To fix this add ``#define _CRT_SECURE_NO_WARNINGS`` macro at the top of your main code. Or right click to your project and go to ``Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions`` and add ``_CRT_SECURE_NO_WARNINGS``.
