# HephAudio [![Static Badge](https://img.shields.io/badge/License-LGPL%20v2.1-blue)](https://github.com/ozguronsoy/HephAudio/blob/master/LICENSE.md) [![GitHub Release](https://img.shields.io/github/v/release/ozguronsoy/HephAudio)](https://github.com/ozguronsoy/HephAudio/releases/)

[![GitHub Pages](https://github.com/ozguronsoy/HephAudio/actions/workflows/github_pages.yml/badge.svg)](https://github.com/ozguronsoy/HephAudio/actions/workflows/github_pages.yml)
[![Build Windows](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_windows.yml/badge.svg)](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_windows.yml)
[![Build Linux](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_linux.yml/badge.svg)](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_linux.yml)
[![Build Android](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_android.yml/badge.svg)](https://github.com/ozguronsoy/HephAudio/actions/workflows/build_android.yml)

- [Setup](#setup)<br>
	- [Install Dependencies](#install-dependencies)<br>
	- [CMake](#cmake)<br>
	- [Visual Studio](#visual-studio)<br>
- Getting Started<br>
	- [Playing Files](https://github.com/ozguronsoy/HephAudio/tree/master/docs/examples/PlayingFiles.md)<br>
	- [Recording](https://github.com/ozguronsoy/HephAudio/tree/master/docs/examples/Recording.md)<br>
	- [Device Enumeration](https://github.com/ozguronsoy/HephAudio/tree/master/docs/examples/DeviceEnumeration.md)<br>
	- [Audio Effects](https://github.com/ozguronsoy/HephAudio/tree/master/docs/examples/Effects.md)<br>
	- [Handling Exceptions](https://github.com/ozguronsoy/HephAudio/tree/master/docs/examples/HandlingExceptions.md)<br>
- [Documentation](https://ozguronsoy.github.io/HephAudio/)<br>
- [Examples](https://github.com/ozguronsoy/HephAudio/tree/master/docs/examples)<br>

## Introduction
HephAudio is a cross-platform audio library that provides:<br>
- Playing and recording audio in Windows, Linux, Android, iOS, and macOS.<br>
- Audio device enumeration and selection.<br>
- Encoding, decoding, and transcoding audio files via [FFmpeg](https://ffmpeg.org/).<br>
- FFT for frequency analysis of the audio signals.<br>
- Spatialization via HRTF.<br>
- Easy to use sound effects and filters.<br>

## Setup

### Install Dependencies

- Install [CMake](https://cmake.org/download/) 3.28.0 or higher.<br>
- Install ALSA development library for **Linux**.<br>
- build FFmpeg and libmysofa for **macOS** and **iOS**.<br>


### CMake

#### Create Shared/Static Library
1) Clone the repo.<br>
2) Run one of the following commands:
	- to create **shared** library: ``cmake -DENABLE_SHARED=On -DCMAKE_CXX_FLAGS='-DHEPHAUDIO_INFO_LOGGING' .``<br>
	- to create **static** library: ``cmake -DENABLE_STATIC=On -DCMAKE_CXX_FLAGS='-DHEPHAUDIO_INFO_LOGGING' .``<br>
3) Run ``cmake --build .`` then ``cmake --install .``<br>
4) Create a folder at your project's root and name it ``HephAudio`` (/project_root/HephAudio).<br>
5) Copy the contents of the ``build`` folder to ``/project_root/HephAudio``.<br>
6) Create a ``CMakeLists.txt`` file at your project's root folder and build it.<br>

An example cmake file:
```
cmake_minimum_required(VERSION 3.28)

# your project name
project("my_application")

set(HEPHAUDIO_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/HephAudio)

if ((NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY) OR (CMAKE_RUNTIME_OUTPUT_DIRECTORY STREQUAL ""))
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
endif()

add_definitions(-DHEPH_SHARED_LIB)

include_directories(
    ${HEPHAUDIO_DIRECTORY}/include/ffmpeg/
    ${HEPHAUDIO_DIRECTORY}/include/libmysofa/
    ${HEPHAUDIO_DIRECTORY}/include/HephCommon/
    ${HEPHAUDIO_DIRECTORY}/include/HephAudio/
)

add_executable(
    ${CMAKE_PROJECT_NAME}
    main.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")

    target_link_libraries(
        ${CMAKE_PROJECT_NAME}

        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avcodec.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avdevice.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avfilter.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avformat.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/avutil.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/swresample.lib
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/swscale.lib

        ${HEPHAUDIO_DIRECTORY}/lib/libmysofa/zlib.lib
        ${HEPHAUDIO_DIRECTORY}/lib/libmysofa/mysofa.lib

        ${HEPHAUDIO_DIRECTORY}/lib/HephAudio.lib
    )

    # copy the DLL files so they will be in the same folder with the executable.
    install(
        DIRECTORY ${HEPHAUDIO_DIRECTORY}/
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        FILES_MATCHING
        PATTERN "*.dll"
        PATTERN "include" EXCLUDE
        PATTERN "lib" EXCLUDE
    )

else ()

    target_link_libraries(
        ${CMAKE_PROJECT_NAME}

        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavcodec.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavdevice.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavfilter.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavformat.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libavutil.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libswresample.so
        ${HEPHAUDIO_DIRECTORY}/lib/ffmpeg/libswscale.so

        ${HEPHAUDIO_DIRECTORY}/lib/libmysofa/libmysofa.so

        ${HEPHAUDIO_DIRECTORY}/lib/libHephAudio.so
    )

endif()
```


#### Use Directly
1) Create a folder at your project's root and name it ``HephAudio`` (/project_root/HephAudio).<br>
2) Copy the repo to the folder you created.<br>
3) **(WINDOWS ONLY)** Copy the required dll files from the dependencies to the build output folder.
4) Create a ``CMakeLists.txt`` file at your project's root folder and build it.<br>

An example cmake file:

```
cmake_minimum_required(VERSION 3.28)

# your project name
project("my_application")

# execute the HephAudio/CMakeLists.txt file
include(${CMAKE_CURRENT_SOURCE_DIR}/HephAudio/CMakeLists.txt)

add_executable(
    ${CMAKE_PROJECT_NAME}
    ${HEPHAUDIO_SRC}
    # your files
    main.cpp
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    ${HEPHAUDIO_LINK_LIBS}
    # your libs
)

# extra definitions
add_definitions(-DHEPHAUDIO_INFO_LOGGING)
```

<br><br>

### Visual Studio
1) Create a folder at your project's root and name it ``HephAudio`` (/project_root/HephAudio).<br>
2) Copy the repo to the folder you created.<br>
3) Right click to your project, go to ``Configuration Properties -> C/C++ -> General -> Additional Including Directories`` and add the locations of the ``HephCommon/HeaderFiles``, ``HephAudio/HeaderFiles``, ``dependencies/ffmpeg/include``, and ``dependencies/libmysofa/include``.<br>
4) Now right click the solution and go to ``Add -> Existing Project``, under the HephCommon folder select ``HephCommon.vcxitems`` to add to your project. Repeat the same process for HephAudio.<br>
5) Right click to your project, ``Add -> Reference -> Shared Projects`` and check both HephAudio and HephCommon.<br>
6) Right click to your project, go to ``Configuration Properties -> Linker -> General -> Additional Library Directories`` and add ``path_to_hephaudio/dependencies``.<br>
7) Copy the required dll files from the dependencies to the build output folder.<br>
8) Visual studio marks some of the standard C functions as unsafe and prevents from compiling by throwing errors. To fix this, right click to your project and go to ``Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions`` and add ``_CRT_SECURE_NO_WARNINGS``.<br>
9) If you are creating a DLL, add ``HEPH_EXPORTS`` and ``HEPH_SHARED_LIB`` preprocessor definitions.

> [!NOTE]
> Don't define ``HEPH_EXPORTS`` when using the DLL.