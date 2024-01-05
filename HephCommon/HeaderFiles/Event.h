#pragma once
#include "HephCommonFramework.h"
#include "EventArgs.h"
#include "EventResult.h"
#include "EventParams.h"
#include <vector>
#include <cinttypes>

namespace HephCommon
{
	typedef void (*EventHandler)(const EventParams& eventParams);
	class Event final
	{
	private:
		std::vector<EventHandler> eventHandlers;
		std::vector<void*> userEventArgs;
	public:
		Event();
		Event(const EventHandler& handler);
		operator bool() const;
		void operator()(EventArgs* pArgs, EventResult* pResult) const;
		Event& operator=(EventHandler handler);
		Event& operator+=(EventHandler handler);
		Event& operator-=(EventHandler handler);
		size_t EventHandlerCount() const;
		EventHandler GetEventHandler(size_t index) const;
		void SetEventHandler(EventHandler handler);
		void AddEventHandler(EventHandler handler);
		void InsertEventHandler(EventHandler handler, size_t index);
		void RemoveEventHandler(EventHandler handler);
		void RemoveEventHandler(size_t index);
		void ClearEventHandlers();
		size_t UserArgCount() const;
		void* GetUserArg(size_t index) const;
		void SetUserArg(void* pUserArg);
		void AddUserArg(void* pUserArg);
		void InsertUserArg(void* pUserArg, size_t index);
		void RemoveUserArg(void* pUserArg);
		void RemoveUserArg(size_t index);
		void ClearUserArgs();
		void ClearAll();
		void Invoke(EventArgs* pArgs, EventResult* pResult) const;
	};
}