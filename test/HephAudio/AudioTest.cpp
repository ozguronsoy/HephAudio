#include "gtest/gtest.h"
#include "Audio.h"
#include "NativeAudio/AndroidAudioA.h"
#include "NativeAudio/AndroidAudioSLES.h"
#include "NativeAudio/AppleAudio.h"
#include "NativeAudio/LinuxAudio.h"
#include "NativeAudio/WinAudio.h"
#include "NativeAudio/WinAudioDS.h"
#include "NativeAudio/WinAudioMME.h"
#include "Exceptions/InvalidArgumentException.h"
#include "TestFiles.h"

using namespace Heph;
using namespace HephAudio;
using namespace HephAudio::Native;

TEST(AudioTest, Constructors)
{
	{
		Audio audio;
		EXPECT_TRUE(audio.GetNativeAudio() != nullptr);
		EXPECT_TRUE(audio.GetAudioDecoder() != nullptr);
		EXPECT_EQ(&audio.OnAudioDeviceAdded, &audio.GetNativeAudio()->OnAudioDeviceAdded);
		EXPECT_EQ(&audio.OnAudioDeviceRemoved, &audio.GetNativeAudio()->OnAudioDeviceRemoved);
		EXPECT_EQ(&audio.OnCapture, &audio.GetNativeAudio()->OnCapture);
	}

#if defined(_WIN32)

	{
		Audio audio;
		EXPECT_TRUE(dynamic_cast<WinAudio*>(audio.GetNativeAudio().get()) != nullptr);
	}

	{
		Audio audio(AudioAPI::WASAPI);
		EXPECT_TRUE(dynamic_cast<WinAudio*>(audio.GetNativeAudio().get()) != nullptr);
	}

	{
		Audio audio(AudioAPI::DirectSound);
		EXPECT_TRUE(dynamic_cast<WinAudioDS*>(audio.GetNativeAudio().get()) != nullptr);
	}

	{
		Audio audio(AudioAPI::MMEAPI);
		EXPECT_TRUE(dynamic_cast<WinAudioMME*>(audio.GetNativeAudio().get()) != nullptr);
	}

#elif defined(__ANDROID__)

#if __ANDROID_API__ >= HEPHAUDIO_ANDROID_AAUDIO_MIN_API_LEVEL

	{
		Audio audio;
		EXPECT_TRUE(dynamic_cast<AndroidAudioA*>(audio.GetNativeAudio().get()) != nullptr);
	}

	{
		Audio audio(AudioAPI::AAudio);
		EXPECT_TRUE(dynamic_cast<AndroidAudioA*>(audio.GetNativeAudio().get()) != nullptr);
	}

	{
		Audio audio(AudioAPI::OpenSLES);
		EXPECT_TRUE(dynamic_cast<AndroidAudioSLES*>(audio.GetNativeAudio().get()) != nullptr);
	}

#else

	{
		Audio audio;
		EXPECT_TRUE(dynamic_cast<AndroidAudioSLES*>(audio.GetNativeAudio().get()) != nullptr);
	}

	{
		Audio audio(AudioAPI::OpenSLES);
		EXPECT_TRUE(dynamic_cast<AndroidAudioSLES*>(audio.GetNativeAudio().get()) != nullptr);
	}

#endif

#elif defined(__linux__)

	{
		Audio audio;
		EXPECT_TRUE(dynamic_cast<LinuxAudio*>(audio.GetNativeAudio().get()) != nullptr);
	}

	{
		Audio audio(AudioAPI::ALSA);
		EXPECT_TRUE(dynamic_cast<LinuxAudio*>(audio.GetNativeAudio().get()) != nullptr);
	}

#elif defined(__APPLE__)

	{
		Audio audio;
		EXPECT_TRUE(dynamic_cast<AppleAudio*>(audio.GetNativeAudio().get()) != nullptr);
	}

	{
		Audio audio(AudioAPI::CoreAudio);
		EXPECT_TRUE(dynamic_cast<AppleAudio*>(audio.GetNativeAudio().get()) != nullptr);
	}

#endif
}

TEST(AudioTest, DecoderEncoder)
{
	Audio audio;
	EXPECT_THROW(audio.SetAudioDecoder(nullptr), InvalidArgumentException);
	EXPECT_THROW(audio.SetAudioEncoder(nullptr), InvalidArgumentException);
}

TEST(AudioTest, PlayLoad)
{
	Audio audio;
	EXPECT_ANY_THROW(audio.Play(""));
	EXPECT_ANY_THROW(audio.Load(""));

	for (const std::filesystem::path& path : TestFiles::wavFiles)
	{
		if (std::filesystem::exists(path))
		{
			AudioObject* pAudioObject = audio.Play(path, 5);
			EXPECT_EQ(pAudioObject->playCount, 5);
			audio.DestroyAudioObject(pAudioObject);

			pAudioObject = audio.Load(path, 10);
			EXPECT_EQ(pAudioObject->playCount, 10);
			audio.DestroyAudioObject(pAudioObject);
		}
		else
		{
			EXPECT_ANY_THROW(audio.Play(path));
			EXPECT_ANY_THROW(audio.Load(path));
		}
	}
}

TEST(AudioTest, AudioObject)
{
	Audio audio;

	{
		const std::string name = "Hello World!";
		const size_t frameCount = 10;
		const AudioFormatInfo format = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_7_POINT_1, 96000);
		AudioObject* pAudioObject = audio.CreateAudioObject(name, frameCount, format.channelLayout, format.sampleRate);
		
		EXPECT_TRUE(pAudioObject != nullptr);
		EXPECT_EQ(pAudioObject->name, name);
		EXPECT_EQ(pAudioObject->buffer.FrameCount(), frameCount);
		EXPECT_EQ(pAudioObject->buffer.FormatInfo(), format);
		audio.DestroyAudioObject(pAudioObject);
	}

	{
		const size_t frameCount = 10;
		const AudioFormatInfo format = HEPHAUDIO_INTERNAL_FORMAT(HEPHAUDIO_CH_LAYOUT_STEREO, 48000);
		AudioObject* pAudioObject1 = audio.CreateAudioObject("Hello", frameCount, format.channelLayout, format.sampleRate);
		AudioObject* pAudioObject2 = audio.CreateAudioObject("World", frameCount, format.channelLayout, format.sampleRate);
		const Guid id1 = pAudioObject1->id;
		const Guid id2 = pAudioObject2->id;

		EXPECT_EQ(audio.GetAudioObjectCount(), 2);

		EXPECT_TRUE(audio.AudioObjectExists(pAudioObject1));
		EXPECT_TRUE(audio.AudioObjectExists(pAudioObject2));
		EXPECT_TRUE(audio.AudioObjectExists(pAudioObject1->id));
		EXPECT_TRUE(audio.AudioObjectExists(pAudioObject2->id));
		EXPECT_FALSE(audio.AudioObjectExists(nullptr));
		EXPECT_FALSE(audio.AudioObjectExists(Guid::GenerateNew()));

		EXPECT_EQ(audio.GetAudioObject("Hello"), pAudioObject1);
		EXPECT_EQ(audio.GetAudioObject("World"), pAudioObject2);
		EXPECT_EQ(audio.GetAudioObject(0), pAudioObject1);
		EXPECT_EQ(audio.GetAudioObject(1), pAudioObject2);
		EXPECT_EQ(audio.GetAudioObject(pAudioObject1->id), pAudioObject1);
		EXPECT_EQ(audio.GetAudioObject(pAudioObject2->id), pAudioObject2);
		EXPECT_TRUE(audio.GetAudioObject("DNE") == nullptr);
		EXPECT_TRUE(audio.GetAudioObject(Guid::GenerateNew()) == nullptr);
		EXPECT_THROW(audio.GetAudioObject(3), InvalidArgumentException);

		EXPECT_FALSE(audio.DestroyAudioObject(Guid::GenerateNew()));
		EXPECT_FALSE(audio.DestroyAudioObject(nullptr));
		EXPECT_TRUE(audio.DestroyAudioObject(pAudioObject1));
		EXPECT_TRUE(audio.DestroyAudioObject(pAudioObject2->id));

		EXPECT_FALSE(audio.AudioObjectExists(pAudioObject1));
		EXPECT_FALSE(audio.AudioObjectExists(pAudioObject2));
		EXPECT_FALSE(audio.AudioObjectExists(id1));
		EXPECT_FALSE(audio.AudioObjectExists(id2));
	}
}

TEST(AudioTest, ToggleCapture)
{
	Audio audio;
	
	audio.PauseCapture();
	EXPECT_TRUE(audio.IsCapturePaused());
	audio.ResumeCapture();
	EXPECT_FALSE(audio.IsCapturePaused());
	audio.PauseCapture();
	EXPECT_TRUE(audio.IsCapturePaused());
}

TEST(AudioTest, DeviceEnumPeriod)
{
	Audio audio;

	audio.SetDeviceEnumerationPeriod(100);
	EXPECT_EQ(audio.GetDeviceEnumerationPeriod(), 100);
	audio.SetDeviceEnumerationPeriod(300);
	EXPECT_EQ(audio.GetDeviceEnumerationPeriod(), 300);
}