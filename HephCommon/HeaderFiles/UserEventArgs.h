#pragma once
#include "HephCommonShared.h"
#include <string>
#include <unordered_map>

/** @file */

namespace HephCommon
{
	/**
	 * @brief class for passing custom data to the event handlers as key/value pairs.
	 * 
	 * @important This class only stores the pointers to the arguments. So it's your responsibility to ensure that the arguments still exist when handling the events.
	 * 
	 */
	class UserEventArgs final
	{
	private:
		std::unordered_map<std::string, void*> argsMap;

	public:
		/**
		 * gets the argument with the provided key.
		 * 
		 * @return pointer to the user argument if exists, otherwise nullptr.
		 */
		void* operator[](const std::string& key) const;

		/**
		 * gets the number of arguments stored.
		 * 
		 */
		size_t Size() const;

		/**
		 * checks whether an argument exists with the provided key.
		 * 
		 * @return true if the argument exists, otherwise false. 
		 */
		bool Exists(const std::string& key) const;

		void Add(const std::string& key, void* pUserArg);
		void Remove(const std::string& key);

		/**
		 * removes all arguments.
		 * 
		 */
		void Clear();
	};
}