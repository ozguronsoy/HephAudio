#pragma once
#include "HephCommonShared.h"

namespace HephCommon
{
	enum StreamStatus
	{
		Fail = 0,
		Success = 1,
		EndOfStream = 0xFF
	};
}