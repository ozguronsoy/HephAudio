#pragma once
#include <vector>
#include <filesystem>

namespace TestFiles
{
	inline const std::vector<std::filesystem::path> wavFiles =
	{
		"test/TestFiles/sine.wav",
		"../test/TestFiles/sine.wav",
		"../../test/TestFiles/sine.wav",
		"HephAudio/test/TestFiles/sine.wav"
	};
}