#include "AudioEvent.h"
#include "AudioException.h"
#include <memory>

namespace HephAudio
{
	AudioEvent::AudioEvent()
	{
		this->eventHandlers = std::vector<AudioEventHandler>(0);
	}
	AudioEvent::operator bool() const
	{
		return this->eventHandlers.size() > 0;
	}
	void AudioEvent::operator()(AudioEventArgs* pArgs, AudioEventResult* pResult) const
	{
		this->Invoke(pArgs, pResult);
	}
	AudioEvent& AudioEvent::operator=(const AudioEventHandler& handler)
	{
		this->Clear();
		this->AddEventHandler(handler);
		return *this;
	}
	AudioEvent& AudioEvent::operator+=(const AudioEventHandler& handler)
	{
		this->AddEventHandler(handler);
		return *this;
	}
	AudioEvent& AudioEvent::operator-=(const AudioEventHandler& handler)
	{
		this->RemoveEventHandler(handler);
		return *this;
	}
	void AudioEvent::AddEventHandler(AudioEventHandler handler)
	{
		if (handler != nullptr)
		{
			this->eventHandlers.push_back(handler);
		}
	}
	void AudioEvent::RemoveEventHandler(AudioEventHandler handler)
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
	void AudioEvent::RemoveEventHandler(size_t index)
	{
		if (index >= this->eventHandlers.size())
		{
			throw AudioException(E_INVALIDARG, "AudioEvent::RemoveEventHandler", "Index out of bounds.");
		}

		this->eventHandlers.erase(this->eventHandlers.begin() + index);
	}
	size_t AudioEvent::EventHandlerCount() const noexcept
	{
		return this->eventHandlers.size();
	}
	void AudioEvent::Clear()
	{
		this->eventHandlers.clear();
	}
	void AudioEvent::Invoke(AudioEventArgs* pArgs, AudioEventResult* pResult) const
	{
		for (size_t i = 0; i < this->eventHandlers.size(); i++)
		{
			this->eventHandlers.at(i)(pArgs, pResult);
		}
	}
}