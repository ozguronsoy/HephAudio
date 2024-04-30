if (NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 14)
endif ()

if (CMAKE_CXX_STANDARD LESS 14)
    message( FATAL_ERROR "the minimum required C++ standard is 14.")
endif ()

if (NOT DEFINED CMAKE_SIZEOF_VOID_P)
    message( FATAL_ERROR "CMAKE_SIZEOF_VOID_P is not set, call project() before running this cmake.")
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
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/AudioCodecs/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/AudioFormats/*.cpp
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

        file(GLOB HEPH_AUDIO
                ${HEPH_AUDIO}
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/src/*.cpp
        )

        if (CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(
                HEPHAUDIO_LINK_LIBS
                ${HEPHAUDIO_LINK_LIBS}
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/windows/netcdf.lib

                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avcodec.lib
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avformat.lib
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avdevice.lib
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avutil.lib
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/swresample.lib
            )
        else()
            set(
                HEPHAUDIO_LINK_LIBS
                ${HEPHAUDIO_LINK_LIBS}
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/windows/netcdf_x64.lib

                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avcodec_x64.lib
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avformat_x64.lib
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avdevice_x64.lib
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/avutil_x64.lib
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/swresample_x64.lib
            )
        endif()

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
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavfilter.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavformat.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavdevice.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libavutil.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libswresample.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/libswscale.so
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    file(GLOB HEPH_AUDIO
            ${HEPH_AUDIO}
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/src/*.cpp
    )

    set(
        HEPHAUDIO_LINK_LIBS
        asound
	    ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/linux/libnetcdf_c++4.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/linux/libnetcdf.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/linux/libhdf5_hl.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/linux/libhdf5.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/linux/libcurl.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/linux/libz.a
    )

    set(
        HEPHAUDIO_LINK_LIBS
        ${HEPHAUDIO_LINK_LIBS}
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libavcodec.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libavformat.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libavdevice.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libavutil.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/libswresample.so
    )

elseif ((CMAKE_SYSTEM_NAME STREQUAL "Darwin") OR (CMAKE_SYSTEM_NAME STREQUAL "iOS"))
    
    set(
        HEPHAUDIO_LINK_LIBS
        "-framework CoreFoundation"
        "-framework CoreAudio"
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/macos/libnetcdf_c++4.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/macos/libnetcdf.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/macos/libhdf5_hl.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/macos/libhdf5.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/macos/libcurl.a
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/macos/libz.a
    )

    set(
        HEPHAUDIO_LINK_LIBS
        ${HEPHAUDIO_LINK_LIBS}
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavcodec.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavformat.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavdevice.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavutil.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libswresample.so
    )

endif ()