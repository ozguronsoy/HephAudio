#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioEventResult.h"
#include <vector>
#include <cinttypes>

namespace HephAudio
{
	typedef void (*AudioEventHandler)(AudioEventArgs* pArgs, AudioEventResult* pResult);
	class AudioEvent final
	{
	private:
		std::vector<AudioEventHandler> eventHandlers;
	public:
		AudioEvent();
		operator bool() const;
		void operator()(AudioEventArgs* pArgs, AudioEventResult* pResult) const;
		AudioEvent& operator+=(const AudioEventHandler& handler);
		AudioEvent& operator-=(const AudioEventHandler& handler);
		void AddEventHandler(AudioEventHandler handler);
		void RemoveEventHandler(AudioEventHandler handler);
		void RemoveEventHandler(size_t index);
		void Clear();
		size_t EventHandlerCount() const noexcept;
		void Invoke(AudioEventArgs* pArgs, AudioEventResult* pResult) const;
	};
}