# Enabling FFmpeg

HephAudio can only read WAV and AIFF files, and supports very few codecs by default. 
But other codecs and file formats (such as mp3, ogg, flac, alac...) are implemented via [FFmpeg](https://ffmpeg.org/). 
To enable these implementations you need to follow a few simple steps;

- Download and install an [FFmpeg build](https://github.com/BtbN/FFmpeg-Builds/releases), you will need to download the DLL, include (.h), and the .lib files.

> [!NOTE]
> The lib was tested with ***6.1***.

- Include the FFmpeg header files.
For Visual Studio, go to ``Properties`` -> ``C/C++`` -> ``General`` -> ``Additional Include Directories`` and add the include directory.
Example: ``C:\FFmpeg\include``.

- Link the FFmpeg .lib files.
For Visual Studio, go to ``Properties`` -> ``Linker`` -> ``Additional Library Directories`` and add the lib directory.
Example: ``C:\FFmpeg\lib``.

- Add ``HEPHAUDIO_USE_FFMPEG`` to the preprocessor definitons.
For Visual Studio, go to ``Properties`` -> ``C/C++`` ``Preprocessor`` -> ``Preprocessor Definitons``.

Now test it by building the app. If there are any errors due to version difference please contact me via e-mail so i can update the library. 
Or create a pull request if you fixed it yourself.
