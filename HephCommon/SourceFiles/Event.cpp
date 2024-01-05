#include "Event.h"
#include "HephException.h"

namespace HephCommon
{
	Event::Event() : eventHandlers(std::vector<EventHandler>(0)) { }
	Event::Event(const EventHandler& handler) : eventHandlers(std::vector<EventHandler>(1, handler)) {}
	Event::operator bool() const
	{
		return this->eventHandlers.size() > 0;
	}
	void Event::operator()(EventArgs* pArgs, EventResult* pResult) const
	{
		this->Invoke(pArgs, pResult);
	}
	Event& Event::operator=(EventHandler handler)
	{
		this->ClearEventHandlers();
		this->AddEventHandler(handler);
		return *this;
	}
	Event& Event::operator+=(EventHandler handler)
	{
		this->AddEventHandler(handler);
		return *this;
	}
	Event& Event::operator-=(EventHandler handler)
	{
		this->RemoveEventHandler(handler);
		return *this;
	}
	size_t Event::EventHandlerCount() const
	{
		return this->eventHandlers.size();
	}
	EventHandler Event::GetEventHandler(size_t index) const
	{
		if (index >= this->eventHandlers.size())
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Event::GetEventHandler", "Index out of range."));
		}
		return this->eventHandlers[index];
	}
	void Event::SetEventHandler(EventHandler handler)
	{
		this->eventHandlers.clear();
		this->AddEventHandler(handler);
	}
	void Event::AddEventHandler(EventHandler handler)
	{
		if (handler != nullptr)
		{
			this->eventHandlers.push_back(handler);
		}
	}
	void Event::InsertEventHandler(EventHandler handler, size_t index)
	{
		if (index > this->eventHandlers.size())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Event::GetEventHandler", "Index out of range."));
			index = this->eventHandlers.size();
		}
		this->eventHandlers.insert(this->eventHandlers.begin() + index, handler);
	}
	void Event::RemoveEventHandler(EventHandler handler)
	{
		for (size_t i = 0; i < this->eventHandlers.size(); i++)
		{
			if (this->eventHandlers.at(i) == handler)
			{
				this->eventHandlers.erase(this->eventHandlers.begin() + i);
				return;
			}
		}
	}
	void Event::RemoveEventHandler(size_t index)
	{
		if (index >= this->eventHandlers.size())
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Event::RemoveEventHandler", "Index out of bounds."));
		}

		this->eventHandlers.erase(this->eventHandlers.begin() + index);
	}
	void Event::ClearEventHandlers()
	{
		this->eventHandlers.clear();
	}
	size_t Event::UserArgCount() const 
	{
		return this->userEventArgs.size();
	}
	void* Event::GetUserArg(size_t index) const 
	{
		if (index >= this->userEventArgs.size())
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Event::GetUserArg", "Index out of range."));
		}
		return this->userEventArgs[index];
	}
	void Event::SetUserArg(void* pUserArg)
	{
		this->userEventArgs.clear();
		this->AddUserArg(pUserArg);
	}
	void Event::AddUserArg(void* pUserArg)
	{
		this->userEventArgs.push_back(pUserArg);
	}
	void Event::InsertUserArg(void* pUserArg, size_t index) 
	{
		if (index > this->userEventArgs.size())
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Event::InsertUserArg", "Index out of range."));
			index = this->userEventArgs.size();
		}
		this->userEventArgs.insert(this->userEventArgs.begin() + index, pUserArg);
	}
	void Event::RemoveUserArg(void* pUserArg) 
	{
		for (size_t i = 0; i < this->userEventArgs.size(); i++)
		{
			if (pUserArg == this->userEventArgs[i])
			{
				this->userEventArgs.erase(this->userEventArgs.begin() + i);
				return;
			}
		}
	}
	void Event::RemoveUserArg(size_t index) 
	{
		if (index >= this->userEventArgs.size())
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Event::RemoveUserArg", "Index out of range."));
		}
		this->userEventArgs.erase(this->userEventArgs.begin() + index);
	}
	void Event::ClearUserArgs() 
	{
		this->userEventArgs.clear();
	}
	void Event::ClearAll()
	{
		this->userEventArgs.clear();
		this->eventHandlers.clear();
	}
	void Event::Invoke(EventArgs* pArgs, EventResult* pResult) const
	{
		EventArgs defaultArgs;
		EventResult defaultResult;

		EventParams eventParams(this->userEventArgs);
		eventParams.pArgs = pArgs != nullptr ? pArgs : &defaultArgs;
		eventParams.pResult = pResult != nullptr ? pResult : &defaultResult;

		for (size_t i = 0; i < this->eventHandlers.size() && !eventParams.pResult->isHandled; i++)
		{
			this->eventHandlers.at(i)(eventParams);
		}
	}
}