#pragma once
#include "HephShared.h"
#include "EventArgs.h"
#include "EventResult.h"
#include "EventParams.h"
#include "UserEventArgs.h"
#include <vector>
#include <cinttypes>

/** @file */

namespace Heph
{
	/**
	 * method for handling events.
	 * 
	 */
	typedef void (*EventHandler)(const EventParams& eventParams);

	/**
	 * @brief class for managing callback functions.
	 * 
	 */
	class HEPH_API Event final
	{
	private:
		std::vector<EventHandler> eventHandlers;

	public:
		/** @copydoc UserEventArgs */
		UserEventArgs userEventArgs;

	public:
		/**
		 * creates a new instance and initializes it with default values.
		 * 
		 */
		Event();
		
		operator bool() const;

		/**
		 * raises the event.
		 * 
		 * @param pArgs pointer to the event args or nullptr.
		 * @param pResult pointer to the event result or nullptr.
		 */
		void operator()(EventArgs* pArgs, EventResult* pResult) const;

		Event& operator=(EventHandler handler);
		Event& operator+=(EventHandler handler);
		Event& operator-=(EventHandler handler);

		/**
		 * gets the number of event handlers that are registered to the current instance.
		 * 
		 */
		size_t EventHandlerCount() const;

		/**
		 * checks whether the provided event handler is registered.
		 * 
		 * @return true if the internal list contanis the provided event handler, otherwise false.
		 */
		bool EventHandlerExists(EventHandler handler) const;

		/**
		 * gets the event handler at the provided index.
		 * 
		 */
		EventHandler GetEventHandler(size_t index) const;

		/**
		 * removes all the event handlers than adds the provided one.
		 * 
		 */
		void SetEventHandler(EventHandler handler);

		/**
		 * adds the provided event handler to the end of the list.
		 * 
		 */
		void AddEventHandler(EventHandler handler);

		/**
		 * inserts the provided event handler to the provided index.
		 * 
		 */
		void InsertEventHandler(EventHandler handler, size_t index);

		/**
		 * removes the provided event handler.
		 * 
		 */
		void RemoveEventHandler(EventHandler handler);

		/**
		 * removes the event handler at the provided index.
		 * 
		 */
		void RemoveEventHandler(size_t index);

		/**
		 * removes all event handlers.
		 * 
		 */
		void ClearEventHandlers();

		/**
		 * removes all event handlers and the user args.
		 * 
		 */
		void ClearAll();

		/**
		 * raises the event.
		 * 
		 * @param pArgs pointer to the event args or nullptr.
		 * @param pResult pointer to the event result or nullptr.
		 */
		void Invoke(EventArgs* pArgs, EventResult* pResult) const;
	};
}