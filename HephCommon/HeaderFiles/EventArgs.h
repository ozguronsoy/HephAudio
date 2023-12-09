#pragma once

namespace HephCommon
{
	struct EventArgs
	{
		const void* pSender;
		EventArgs(const void* pSender) : pSender(pSender) {}
		virtual ~EventArgs() = default;
	};
}