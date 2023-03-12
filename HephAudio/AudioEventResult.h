#pragma once
#include "framework.h"

namespace HephAudio
{
	/// <summary>
	/// Base class for returning data from events.
	/// </summary>
	struct AudioEventResult
	{
		virtual ~AudioEventResult() = default;
	};
}