#pragma once
#include "framework.h"
#include "AudioEventArgs.h"
#include "AudioEventResult.h"
#include <vector>
#include <cinttypes>

namespace HephAudio
{
	/// <summary>
	/// The prototype of the function that's called when an audio event occurs.
	/// </summary>
	typedef void (*AudioEventHandler)(AudioEventArgs* pArgs, AudioEventResult* pResult);
	/// <summary>
	/// Provides methods for assigning multiple callback functions to handle a specific case.
	/// </summary>
	class AudioEvent final
	{
	private:
		std::vector<AudioEventHandler> eventHandlers;
	public:
		/// <summary>
		/// Creates and initializes an AudioEvent instance.
		/// </summary>
		AudioEvent();
		/// <summary>
		/// true if the number of event handlers is greater than 0.
		/// </summary>
		operator bool() const;
		/// <summary>
		/// Calls the event handler methods in the order they were added.
		/// </summary>
		/// <param name="pArgs">Pointer to the necessary information to handle the event.</param>
		/// <param name="pResult">Pointer to the information that will be used after the event is handled.</param>
		void operator()(AudioEventArgs* pArgs, AudioEventResult* pResult) const;
		/// <summary>
		/// Sets the provided method as the only event handler.
		/// </summary>
		/// <param name="handler">The method that's called when an event occurs.</param>
		AudioEvent& operator=(const AudioEventHandler& handler);
		/// <summary>
		/// Adds the provided method to the event handlers.
		/// </summary>
		/// <param name="handler">The method that's called when an event occurs.</param>
		AudioEvent& operator+=(const AudioEventHandler& handler);
		/// <summary>
		/// Removes the method from the event handlers.
		/// </summary>
		/// <param name="handler">The method to remove.</param>
		AudioEvent& operator-=(const AudioEventHandler& handler);
		/// <summary>
		/// Adds the provided method to the event handlers.
		/// </summary>
		/// <param name="handler">The method that's called when an event occurs.</param>
		void AddEventHandler(AudioEventHandler handler);
		/// <summary>
		/// Removes the method from the event handlers.
		/// </summary>
		/// <param name="handler">The method to remove.</param>
		void RemoveEventHandler(AudioEventHandler handler);
		/// <summary>
		/// Removes the method from the event handlers at the provided index.
		/// </summary>
		/// <param name="index">The index of the event handler to remove.</param>
		void RemoveEventHandler(size_t index);
		/// <summary>
		/// Removes all of the event handlers.
		/// </summary>
		void Clear();
		/// <summary>
		/// Calculates the number of event handlers.
		/// </summary>
		/// <returns>The number of event handlers.</returns>
		size_t EventHandlerCount() const noexcept;
		/// <summary>
		/// Calls the event handler methods in the order they were added.
		/// </summary>
		/// <param name="pArgs">Pointer to the necessary information to handle the event.</param>
		/// <param name="pResult">Pointer to the information that will be used after the event is handled.</param>
		void Invoke(AudioEventArgs* pArgs, AudioEventResult* pResult) const;
	};
}