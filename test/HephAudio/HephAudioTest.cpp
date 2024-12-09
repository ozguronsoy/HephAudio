#include "gtest/gtest.h"
#include "HephAudioShared.h"
#include "StringHelpers.h"
#include <cmath>

using namespace Heph;
using namespace HephAudio;

TEST(HephAudioSharedTest, Versions)
{
	const std::string expectedVersion =
		"v" +
		StringHelpers::ToString(HEPHAUDIO_VERSION_MAJOR) +
		"." +
		StringHelpers::ToString(HEPHAUDIO_VERSION_MINOR) +
		"." +
		StringHelpers::ToString(HEPHAUDIO_VERSION_PATCH);

	EXPECT_EQ(HEPHAUDIO_VERSION, expectedVersion);
	EXPECT_EQ(HephAudio::GetVersion(), expectedVersion);
	EXPECT_EQ(HEPHAUDIO_VERSION_MAJOR, HephAudio::GetVersionMajor());
	EXPECT_EQ(HEPHAUDIO_VERSION_MINOR, HephAudio::GetVersionMinor());
	EXPECT_EQ(HEPHAUDIO_VERSION_PATCH, HephAudio::GetVersionPatch());
}

TEST(HephAudioSharedTest, S2F)
{
	EXPECT_EQ(HEPH_AUDIO_SAMPLE_TO_IEEE_FLT(HEPH_AUDIO_SAMPLE_MAX), 1.0);
	EXPECT_EQ(HEPH_AUDIO_SAMPLE_FROM_IEEE_FLT(1.0), HEPH_AUDIO_SAMPLE_MAX);
}

TEST(HephAudioSharedTest, UnitConversions)
{
	constexpr double absError = 0.01;

	{
		const double expectedGain = 0.707946;
		const double expectedDecibel = -3.0;

		EXPECT_NEAR(HephAudio::DecibelToGain(expectedDecibel), expectedGain, absError);
		EXPECT_NEAR(HephAudio::GainToDecibel(expectedGain), expectedDecibel, absError);
	}

	{
		const double octave = 2;
		const double expectedSemitone = 24;
		const double expectedCent = 2400;

		EXPECT_NEAR(HephAudio::OctaveToCent(octave), expectedCent, absError);
		EXPECT_NEAR(HephAudio::OctaveToSemitone(octave), expectedSemitone, absError);
	}

	{
		const double semitone = 2;
		const double expectedCent = 200;
		const double expectedOctave = 2.0 / 12.0;

		EXPECT_NEAR(HephAudio::SemitoneToOctave(semitone), expectedOctave, absError);
		EXPECT_NEAR(HephAudio::SemitoneToCent(semitone), expectedCent, absError);
	}

	{
		const double cent = 200;
		const double expectedOctave = 200.0 / 1200.0;
		const double expectedSemitone = 2;

		EXPECT_NEAR(HephAudio::CentToOctave(cent), expectedOctave, absError);
		EXPECT_NEAR(HephAudio::CentToSemitone(cent), expectedSemitone, absError);
	}
}