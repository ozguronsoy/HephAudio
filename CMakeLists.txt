if (NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 23)
endif ()

if (CMAKE_CXX_STANDARD LESS 17)
    message( FATAL_ERROR "the minimum required C++ standard is 17.")
endif ()

if (NOT DEFINED CMAKE_SIZEOF_VOID_P)
    message( FATAL_ERROR "CMAKE_SIZEOF_VOID_P is not set, call project() before running this cmake.")
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    message( FATAL_ERROR "CMAKE_SIZEOF_VOID_P is set to 4, 32-bit is not supported.")
endif()

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/HephCommon/HeaderFiles
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/HeaderFiles
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/include
    )

file(GLOB HEPH_AUDIO
        ${CMAKE_CURRENT_LIST_DIR}/HephCommon/SourceFiles/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephCommon/SourceFiles/Buffers/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephCommon/SourceFiles/Exceptions/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/AudioEvents/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/FFmpeg/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/NativeAudio/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/Oscillators/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/Windows/*.cpp
    )

if (DEFINED MSVC)
    link_directories(${CMAKE_CURRENT_LIST_DIR}/dependencies)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")

    set(
        HEPHAUDIO_LINK_LIBS 
        Dsound
        Winmm
    )

    if (NOT DEFINED MSVC)

        set(
            HEPHAUDIO_LINK_LIBS
            ${HEPHAUDIO_LINK_LIBS}
            
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/windows/mingw/mysofa.lib
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/windows/zlib.lib
            
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avcodec.lib
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avformat.lib
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avdevice.lib
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avutil.lib
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/swresample.lib
        )

    endif()

elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
    
     set(
        HEPHAUDIO_LINK_LIBS 
        m
        log
        OpenSLES
    )

    if ((CMAKE_SYSTEM_VERSION GREATER_EQUAL 26) OR (ANDROID_NATIVE_API_LEVEL GREATER_EQUAL 26))
        set(
            HEPHAUDIO_LINK_LIBS
            ${HEPHAUDIO_LINK_LIBS} 
            aaudio
        )
    endif()

    set(
        HEPHAUDIO_LINK_LIBS
        ${HEPHAUDIO_LINK_LIBS}

        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libmysofa.so

        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavfilter.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavformat.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavdevice.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavutil.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libswresample.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libswscale.so
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    set(
        HEPHAUDIO_LINK_LIBS
        asound
	    
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/linux/libmysofa.so

        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libavcodec.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libavformat.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libavdevice.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libavutil.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libswresample.so
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    
    set(
        HEPHAUDIO_LINK_LIBS
        "-framework CoreFoundation"
        "-framework CoreAudio"

        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/macos/libmysofa.so

        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavcodec.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavformat.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavdevice.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavutil.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libswresample.so
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "iOS")

    set (
            HEPHAUDIO_LINK_LIBS
            "-framework CoreFoundation"
            "-framework CoreAudio"

            ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/ios/libmysofa.so

            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavcodec.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavformat.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavdevice.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavutil.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libswresample.so
    )

endif ()