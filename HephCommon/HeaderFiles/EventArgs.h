#pragma once

namespace HephCommon
{
	struct EventArgs
	{
		const void* pSender;
		bool isHandled;
		EventArgs(const void* pSender) : pSender(pSender), isHandled(false) {}
		virtual ~EventArgs() = default;
	};
}