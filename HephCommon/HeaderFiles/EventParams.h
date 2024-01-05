#pragma once
#include "HephCommonFramework.h"
#include "EventArgs.h"
#include "EventResult.h"
#include "StringBuffer.h"
#include "UserEventArgs.h"
#include <unordered_map>

namespace HephCommon
{
	struct EventParams final
	{
		EventArgs* pArgs;
		EventResult* pResult;
		const UserEventArgs& userEventArgs;
		EventParams(const UserEventArgs& userEventArgs) : pArgs(nullptr), pResult(nullptr), userEventArgs(userEventArgs) {}
	};
}