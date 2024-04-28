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
else()
    file(GLOB HEPH_AUDIO
        ${HEPH_AUDIO}
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/src/*.cpp
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(
        HEPHAUDIO_LINK_LIBS 
        Dsound
        Winmm
    )

    if (NOT DEFINED MSVC)

        if (CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(
                HEPHAUDIO_LINK_LIBS
                ${HEPHAUDIO_LINK_LIBS}
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/win/netcdf.lib
            )
        else()
            set(
                HEPHAUDIO_LINK_LIBS
                ${HEPHAUDIO_LINK_LIBS}
                ${CMAKE_CURRENT_LIST_DIR}/dependencies/libsofa/lib/win/netcdf_x64.lib
            )
        endif()

    endif()

    if (DEFINED HEPH_FFMPEG_ROOT)
        include_directories(${HEPH_FFMPEG_ROOT}/include)
        set(
            HEPHAUDIO_LINK_LIBS
            ${HEPHAUDIO_LINK_LIBS}
            ${HEPH_FFMPEG_ROOT}/lib/avcodec.lib
            ${HEPH_FFMPEG_ROOT}/lib/avformat.lib
            ${HEPH_FFMPEG_ROOT}/lib/avutil.lib
            ${HEPH_FFMPEG_ROOT}/lib/swresample.lib
        )
        add_definitions(-DHEPHAUDIO_USE_FFMPEG)
    endif ()

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

    if (DEFINED HEPH_FFMPEG_ROOT)
        include_directories(${HEPH_FFMPEG_ROOT}/include)
        set(
            HEPHAUDIO_LINK_LIBS
            ${HEPHAUDIO_LINK_LIBS}
            ${HEPH_FFMPEG_ROOT}/lib/libavcodec.so
            ${HEPH_FFMPEG_ROOT}/lib/libavformat.so
            ${HEPH_FFMPEG_ROOT}/lib/libavutil.so
            ${HEPH_FFMPEG_ROOT}/lib/libswresample.so
        )
        add_definitions(-DHEPHAUDIO_USE_FFMPEG)
    endif ()

elseif  (CMAKE_SYSTEM_NAME STREQUAL "Linux")

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

    if (DEFINED HEPH_FFMPEG_ROOT)
        include_directories(${HEPH_FFMPEG_ROOT}/include)
        set(
            HEPHAUDIO_LINK_LIBS
            ${HEPHAUDIO_LINK_LIBS}
            ${HEPH_FFMPEG_ROOT}/lib/libavcodec.so
            ${HEPH_FFMPEG_ROOT}/lib/libavformat.so
            ${HEPH_FFMPEG_ROOT}/lib/libavutil.so
            ${HEPH_FFMPEG_ROOT}/lib/libswresample.so
        )
        add_definitions(-DHEPHAUDIO_USE_FFMPEG)
    endif ()

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

    if (DEFINED HEPH_FFMPEG_ROOT)
        include_directories(${HEPH_FFMPEG_ROOT}/include)
        set(
            HEPHAUDIO_LINK_LIBS
            ${HEPHAUDIO_LINK_LIBS}
            ${HEPH_FFMPEG_ROOT}/lib/libavcodec.so
            ${HEPH_FFMPEG_ROOT}/lib/libavformat.so
            ${HEPH_FFMPEG_ROOT}/lib/libavutil.so
            ${HEPH_FFMPEG_ROOT}/lib/libswresample.so
        )
        add_definitions(-DHEPHAUDIO_USE_FFMPEG)
    endif ()
endif ()