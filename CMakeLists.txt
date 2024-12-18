cmake_minimum_required(VERSION 3.28)

set(HEPHAUDIO_VERSION_MAJOR 3)
set(HEPHAUDIO_VERSION_MINOR 1)
set(HEPHAUDIO_VERSION_PATCH 0)
set(HEPHAUDIO_VERSION ${HEPHAUDIO_VERSION_MAJOR}.${HEPHAUDIO_VERSION_MINOR}.${HEPHAUDIO_VERSION_PATCH})

option(ENABLE_STATIC "ENABLE_STATIC" Off)
option(ENABLE_SHARED "ENABLE_SHARED" Off)
option(ENABLE_TESTS "ENABLE_TESTS" Off)

if (NOT DEFINED HEPHAUDIO_BUILD_DIR)
    set(HEPHAUDIO_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 26)
endif ()

if (CMAKE_CXX_STANDARD LESS 17)
    message( FATAL_ERROR "the minimum required C++ standard is 17.")
endif ()

if (NOT DEFINED CMAKE_SIZEOF_VOID_P)
    project("HephAudio" VERSION ${HEPHAUDIO_VERSION} LANGUAGES CXX)
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    message( FATAL_ERROR "CMAKE_SIZEOF_VOID_P is set to 4, 32-bit is not supported.")
endif()

if ((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64") OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"))
    set(TARGET_ARCH "x86_64")
elseif ((CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64") OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64"))
    set(TARGET_ARCH "arm64-v8a")
else()
    message( FATAL_ERROR "unsupported arch.")
endif()

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/HephCommon/HeaderFiles
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/HeaderFiles
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/include
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/include
    )

file(GLOB HEPHAUDIO_SRC
        ${CMAKE_CURRENT_LIST_DIR}/HephCommon/SourceFiles/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephCommon/SourceFiles/Buffers/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephCommon/SourceFiles/Exceptions/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/HephAudio/SourceFiles/AudioEffects/*.cpp
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

    set(CMAKE_IMPORT_LIBRARY_PREFIX "")
    set(CMAKE_IMPORT_LIBRARY_SUFFIX ".lib")
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
    set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
    set(CMAKE_SHARED_LIBRARY_PREFIX "")

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

    if (ENABLE_STATIC OR ENABLE_SHARED)
        
        install(
            DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/
            DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/ffmpeg
            FILES_MATCHING 
            PATTERN "*.lib"
            PATTERN "dlls" EXCLUDE
        )
        install(
            FILES ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/windows/zlib.lib 
            DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/libmysofa
        )

        if (DEFINED MSVC)
            install(
                FILES ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/windows/msvc/mysofa.lib 
                DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/libmysofa
            )
        else()
            install(
                FILES ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/windows/mingw/mysofa.lib 
                DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/libmysofa
            )
        endif()

    endif()

    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/windows/dlls/ 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}
        FILES_MATCHING
        PATTERN "*.dll"
    )
    
    install(
        FILES ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/windows/dlls/zlib.dll 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}
    )

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

    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/android/${CMAKE_ANDROID_ARCH_ABI}/ 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/ffmpeg
    )
    
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/android/${CMAKE_ANDROID_ARCH_ABI}/
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/libmysofa
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    
    set(
        HEPHAUDIO_LINK_LIBS
        asound
	    
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/linux/libmysofa.so

        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/${TARGET_ARCH}/libavcodec.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/${TARGET_ARCH}/libavfilter.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/${TARGET_ARCH}/libavformat.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/${TARGET_ARCH}/libavdevice.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/${TARGET_ARCH}/libavutil.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/${TARGET_ARCH}/libswresample.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/${TARGET_ARCH}/libswscale.so
    )

    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/linux/${TARGET_ARCH} 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/ffmpeg
    )
    
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/linux/
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/libmysofa
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    
    set(
        HEPHAUDIO_LINK_LIBS
        "-framework CoreFoundation"
        "-framework CoreAudio"

        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/macos/libmysofa.so

        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavcodec.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavfilter.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavformat.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavdevice.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libavutil.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libswresample.so
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/libswscale.so
    )

    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/macos/ 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/ffmpeg
    )
    
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/macos/
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/libmysofa
    )

elseif (CMAKE_SYSTEM_NAME STREQUAL "iOS")

    set (
            HEPHAUDIO_LINK_LIBS
            "-framework CoreFoundation"
            "-framework CoreAudio"

            ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/ios/libmysofa.so

            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavcodec.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavfilter.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavformat.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavdevice.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libavutil.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libswresample.so
            ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/libswscale.so
    )

    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/lib/ios/ 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/ffmpeg
    )
    
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/lib/ios/
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib/libmysofa
    )

else()

    message( FATAL_ERROR "Unsupported platform.")

endif ()

if (ENABLE_STATIC OR ENABLE_SHARED)

    if (ENABLE_SHARED)
        add_definitions(-DHEPH_SHARED_LIB -DHEPH_EXPORTS)
        add_library(
            ${CMAKE_PROJECT_NAME} SHARED
            ${HEPHAUDIO_SRC}
        )
    else()
        add_library(
            ${CMAKE_PROJECT_NAME} STATIC
            ${HEPHAUDIO_SRC}
        )
    endif()

    target_link_libraries(
        ${CMAKE_PROJECT_NAME}
        ${HEPHAUDIO_LINK_LIBS}
    )

    set_target_properties(
        ${CMAKE_PROJECT_NAME} PROPERTIES 
        VERSION ${HEPHAUDIO_VERSION}
        SOVERSION ${HEPHAUDIO_VERSION_MAJOR}
    )
    
    install(
        TARGETS ${CMAKE_PROJECT_NAME} 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}
    )

    install(
        TARGETS ${CMAKE_PROJECT_NAME} 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/lib
    )
    
    install(
        FILES ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/default.sofa 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}
    )
    install(
        FILES ${CMAKE_CURRENT_LIST_DIR}/LICENSE.md 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}
    )
    
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/HephCommon/HeaderFiles/ 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/include/HephCommon
    )
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/HephAudio/HeaderFiles/ 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/include/HephAudio
    )
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/ffmpeg/include/ 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/include/ffmpeg
    )
    install(
        DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmysofa/include/ 
        DESTINATION ${HEPHAUDIO_BUILD_DIR}/include/libmysofa
    )

endif ()

if (ENABLE_TESTS)

    set(HEPHAUDIO_TEST "hephaudio_test")

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(googletest)

    include_directories(
        ${GTEST_INCLUDE_DIRS}
        ${CMAKE_CURRENT_LIST_DIR}/test/HephCommon
        ${CMAKE_CURRENT_LIST_DIR}/test/HephAudio
    )

    file(GLOB HEPHAUDIO_TEST_SRC
        ${CMAKE_CURRENT_LIST_DIR}/test/HephCommon/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/test/HephAudio/*.cpp
    )

    add_executable(
        ${HEPHAUDIO_TEST}
        ${HEPHAUDIO_SRC}
        ${HEPHAUDIO_TEST_SRC}
    )

    target_link_libraries(
        ${HEPHAUDIO_TEST} 
        ${HEPHAUDIO_LINK_LIBS}
        gtest 
        gtest_main
    )

endif ()