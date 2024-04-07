#pragma once
#include "HephCommonShared.h"
#include "EventArgs.h"
#include "EventResult.h"
#include "UserEventArgs.h"

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