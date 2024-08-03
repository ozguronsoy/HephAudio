#include "IAudioDecoder.h"

namespace HephAudio
{
	IAudioDecoder::IAudioDecoder() : IAudioDecoder("") {}
	IAudioDecoder::IAudioDecoder(const std::string& filePath) : filePath(filePath) {}
}