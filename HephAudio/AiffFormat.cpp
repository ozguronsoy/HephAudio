#include "AiffFormat.h"
#include "AudioException.h"
#include "AudioProcessor.h"

namespace HephAudio
{
	namespace Formats
	{
		StringBuffer AiffFormat::Extension() const
		{
			return ".aiff .aifc .aif";
		}
		AudioFormatInfo AiffFormat::ReadAudioFormatInfo(const AudioFile* pAudioFile) const
		{
			return AudioFormatInfo();
		}
		AudioBuffer AiffFormat::ReadFile(const AudioFile* pAudioFile) const
		{
			return AudioBuffer();
		}
		bool AiffFormat::SaveToFile(StringBuffer filePath, AudioBuffer& buffer, bool overwrite) const
		{
			return true;
		}
		void AiffFormat::SampleRateFrom64(uint64_t srBits, AudioFormatInfo* format) const
		{
			if (srBits == 0x400FAC4400000000)
			{
				format->sampleRate = 88200;
			}
			else if (srBits == 0x400EAC4400000000)
			{
				format->sampleRate = 44100;
			}
			else if (srBits == 0x400DAC4400000000)
			{
				format->sampleRate = 22050;
			}
			else if (srBits == 0x400CAC4400000000)
			{
				format->sampleRate = 11025;
			}
			else if (srBits == 0x400FBB8000000000)
			{
				format->sampleRate = 96000;
			}
			else if (srBits == 0x400EBB8000000000)
			{
				format->sampleRate = 48000;
			}
			else if (srBits == 0x400DBB8000000000)
			{
				format->sampleRate = 24000;
			}
			else if (srBits == 0x400CBB8000000000)
			{
				format->sampleRate = 12000;
			}
			else if (srBits == 0x400BBB8000000000)
			{
				format->sampleRate = 6000;
			}
			else if (srBits == 0x400FFA0000000000)
			{
				format->sampleRate = 128000;
			}
			else if (srBits == 0x400EFA0000000000)
			{
				format->sampleRate = 64000;
			}
			else if (srBits == 0x400DFA0000000000)
			{
				format->sampleRate = 32000;
			}
			else if (srBits == 0x400CFA0000000000)
			{
				format->sampleRate = 16000;
			}
			else if (srBits == 0x400BFA0000000000)
			{
				format->sampleRate = 8000;
			}
			else if (srBits == 0x400AFA0000000000)
			{
				format->sampleRate = 4000;
			}
			else if (srBits == 0x4009FA0000000000)
			{
				format->sampleRate = 2000;
			}
			else if (srBits == 0x4008FA0000000000)
			{
				format->sampleRate = 1000;
			}
			else
			{
				throw AudioException(E_FAIL, L"AiffFormat", L"Unknown sample rate.");
			}
		}
		uint64_t AiffFormat::SampleRateTo64(const AudioFormatInfo* const& format) const
		{
			if (format->sampleRate == 88200)
			{
				return 0x400FAC4400000000;
			}
			else if (format->sampleRate == 44100)
			{
				return 0x400EAC4400000000;
			}
			else if (format->sampleRate == 22050)
			{
				return 0x400DAC4400000000;
			}
			else if (format->sampleRate == 11025)
			{
				return 0x400CAC4400000000;
			}
			else if (format->sampleRate == 96000)
			{
				return 0x400FBB8000000000;
			}
			else if (format->sampleRate == 48000)
			{
				return 0x400EBB8000000000;
			}
			else if (format->sampleRate == 24000)
			{
				return 0x400DBB8000000000;
			}
			else if (format->sampleRate == 12000)
			{
				return 0x400CBB8000000000;
			}
			else if (format->sampleRate == 6000)
			{
				return 0x400BBB8000000000;
			}
			else if (format->sampleRate == 128000)
			{
				return 0x400FFA0000000000;
			}
			else if (format->sampleRate == 64000)
			{
				return 0x400EFA0000000000;
			}
			else if (format->sampleRate == 32000)
			{
				return 0x400DFA0000000000;
			}
			else if (format->sampleRate == 16000)
			{
				return 0x400CFA0000000000;
			}
			else if (format->sampleRate == 8000)
			{
				return 0x400BFA0000000000;
			}
			else if (format->sampleRate == 4000)
			{
				return 0x400AFA0000000000;
			}
			else if (format->sampleRate == 2000)
			{
				return 0x4009FA0000000000;
			}
			else if (format->sampleRate == 1000)
			{
				return 0x4008FA0000000000;
			}
			throw AudioException(E_FAIL, L"AiffFormat", L"Unknown sample rate.");
		}
	}
}