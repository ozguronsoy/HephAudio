#pragma once

namespace HephCommon
{
	struct EventResult
	{
		bool isHandled;
		EventResult() : isHandled(false) {}
		virtual ~EventResult() = default;
	};
}