#include "gtest/gtest.h"
#include "AudioObject.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;
using namespace HephAudio;

TEST(AudioObjectTest, MemberMethods)
{
	const double expectedPosition = 0.5;

	AudioObject ao;
	ao.buffer = AudioBuffer(512, HEPHAUDIO_CH_LAYOUT_STEREO, 48000);

	EXPECT_NO_THROW(ao.SetPosition(expectedPosition));
	EXPECT_EQ(ao.GetPosition(), expectedPosition);
	EXPECT_THROW(ao.SetPosition(-0.5), InvalidArgumentException);
	EXPECT_THROW(ao.SetPosition(1.2), InvalidArgumentException);

	ao.Pause();
	EXPECT_EQ(ao.isPaused, true);
	ao.Resume();
	EXPECT_EQ(ao.isPaused, false);
	ao.Pause();
	EXPECT_EQ(ao.isPaused, true);
}