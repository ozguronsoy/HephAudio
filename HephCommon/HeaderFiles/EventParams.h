#pragma once
#include "HephCommonFramework.h"
#include "EventArgs.h"
#include "EventResult.h"
#include <vector>

namespace HephCommon
{
	struct EventParams final
	{
		EventArgs* pArgs;
		EventResult* pResult;
		const std::vector<void*>& userEventArgs;
		EventParams(const std::vector<void*>& userEventArgs) : pArgs(nullptr), pResult(nullptr), userEventArgs(userEventArgs) {}
	};
}