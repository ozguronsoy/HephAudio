#pragma once
#include "HephCommonFramework.h"
#include "EventArgs.h"
#include "EventResult.h"
#include <vector>
#include <cinttypes>

namespace HephCommon
{
	typedef void (*EventHandler)(EventArgs* pArgs, EventResult* pResult);
	class Event final
	{
	private:
		std::vector<EventHandler> eventHandlers;
	public:
		Event();
		Event(const EventHandler& handler);
		operator bool() const;
		void operator()(EventArgs* pArgs, EventResult* pResult) const;
		Event& operator=(const EventHandler& handler);
		Event& operator+=(const EventHandler& handler);
		Event& operator-=(const EventHandler& handler);
		void AddEventHandler(EventHandler handler);
		void RemoveEventHandler(EventHandler handler);
		void RemoveEventHandler(size_t index);
		void Clear();
		size_t EventHandlerCount() const noexcept;
		void Invoke(EventArgs* pArgs, EventResult* pResult) const;
	};
}