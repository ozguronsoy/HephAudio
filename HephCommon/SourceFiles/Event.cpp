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
	Event& Event::operator=(const EventHandler& handler)
	{
		this->Clear();
		this->AddEventHandler(handler);
		return *this;
	}
	Event& Event::operator+=(const EventHandler& handler)
	{
		this->AddEventHandler(handler);
		return *this;
	}
	Event& Event::operator-=(const EventHandler& handler)
	{
		this->RemoveEventHandler(handler);
		return *this;
	}
	EventHandler Event::GetEventHandler(size_t index) const
	{
		if (index >= this->eventHandlers.size())
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Event::GetEventHandler", "Index out of range."));
		}
		return this->eventHandlers[index];
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
				this->RemoveEventHandler(i);
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
	size_t Event::EventHandlerCount() const
	{
		return this->eventHandlers.size();
	}
	void Event::Clear()
	{
		this->eventHandlers.clear();
	}
	void Event::Invoke(EventArgs* pArgs, EventResult* pResult) const
	{
		if (pArgs == nullptr)
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_INVALID_ARGUMENT, "Event::Invoke", "pArgs cannot be nullptr."));
		}

		for (size_t i = 0; i < this->eventHandlers.size() && !pArgs->isHandled; i++)
		{
			this->eventHandlers.at(i)(pArgs, pResult);
		}
	}
}