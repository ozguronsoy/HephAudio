#include "Event.h"
#include "HephException.h"

namespace HephCommon
{
	Event::Event() : eventHandlers(std::vector<EventHandler>(0)) { }
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
	bool Event::EventHandlerExists(EventHandler handler) const
	{
		for (size_t i = 0; i < this->eventHandlers.size(); i++)
		{
			if (this->eventHandlers[i] == handler)
			{
				return true;
			}
		}
		return false;
	}
	EventHandler Event::GetEventHandler(size_t index) const
	{
		if (index >= this->eventHandlers.size())
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "Index out of range."));
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
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "Index out of range."));
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
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, HEPH_FUNC, "Index out of bounds."));
		}

		this->eventHandlers.erase(this->eventHandlers.begin() + index);
	}
	void Event::ClearEventHandlers()
	{
		this->eventHandlers.clear();
	}
	void Event::ClearAll()
	{
		this->userEventArgs.Clear();
		this->ClearEventHandlers();
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