#pragma once
#include "HephCommonShared.h"
#include "EventArgs.h"
#include "EventResult.h"
#include "EventParams.h"
#include "UserEventArgs.h"
#include <vector>
#include <cinttypes>
#include <unordered_map>

namespace HephCommon
{
	typedef void (*EventHandler)(const EventParams& eventParams);
	class Event final
	{
	private:
		std::vector<EventHandler> eventHandlers;
	public:
		UserEventArgs userEventArgs;
	public:
		Event();
		Event(const EventHandler& handler);
		operator bool() const;
		void operator()(EventArgs* pArgs, EventResult* pResult) const;
		Event& operator=(EventHandler handler);
		Event& operator+=(EventHandler handler);
		Event& operator-=(EventHandler handler);
		size_t EventHandlerCount() const;
		bool EventHandlerExists(EventHandler handler) const;
		EventHandler GetEventHandler(size_t index) const;
		void SetEventHandler(EventHandler handler);
		void AddEventHandler(EventHandler handler);
		void InsertEventHandler(EventHandler handler, size_t index);
		void RemoveEventHandler(EventHandler handler);
		void RemoveEventHandler(size_t index);
		void ClearEventHandlers();
		void ClearAll();
		void Invoke(EventArgs* pArgs, EventResult* pResult) const;
	};
}