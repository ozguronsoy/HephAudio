cmake_minimum_required(VERSION 3.22.1)

# min supported version is 14
set (CMAKE_CXX_STANDARD 14)
 
# set (FFMPEG_ROOT "/path/to/FFmpeg")

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()

project("myapplication")

include_directories(
    HephAudio/HephCommon/HeaderFiles
    HephAudio/HephAudio/HeaderFiles)

file(GLOB HEPH_AUDIO
    HephAudio/HephCommon/SourceFiles/*.cpp
    HephAudio/HephAudio/SourceFiles/*.cpp
    HephAudio/HephAudio/SourceFiles/AudioCodecs/*.cpp
    HephAudio/HephAudio/SourceFiles/AudioFormats/*.cpp
    HephAudio/HephAudio/SourceFiles/FFmpeg/*.cpp
    HephAudio/HephAudio/SourceFiles/NativeAudio/*.cpp
    HephAudio/HephAudio/SourceFiles/Oscillators/*.cpp
    HephAudio/HephAudio/SourceFiles/Windows/*.cpp)


if (CMAKE_SYSTEM_NAME STREQUAL "Android")

    add_library(
        ${CMAKE_PROJECT_NAME} SHARED
        ${HEPH_AUDIO})

    target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        m
        log
        OpenSLES
    )

    if ((CMAKE_SYSTEM_VERSION GREATER_EQUAL 27) OR (ANDROID_NATIVE_API_LEVEL GREATER_EQUAL 27))
        target_link_libraries(
            ${CMAKE_PROJECT_NAME}
            aaudio
        )
    endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

    add_executable(
        ${CMAKE_PROJECT_NAME}
        ${HEPH_AUDIO})

    target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        Dsound
        Winmm
    )

    if(DEFINED FFMPEG_ROOT)
        include_directories(${FFMPEG_ROOT}/include)
        target_link_libraries(
            ${CMAKE_PROJECT_NAME}
            ${FFMPEG_ROOT}/lib/avcodec.lib
            ${FFMPEG_ROOT}/lib/avformat.lib
            ${FFMPEG_ROOT}/lib/avutil.lib
            ${FFMPEG_ROOT}/lib/swresample.lib
        )
        add_definitions(-DHEPHAUDIO_USE_FFMPEG)
    endif ()

endif ()

message("CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})
message("CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION})
message("FFMPEG_ROOT: " ${FFMPEG_ROOT})
