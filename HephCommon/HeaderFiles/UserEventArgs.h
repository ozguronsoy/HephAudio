#pragma once
#include "HephCommonShared.h"
#include <string>
#include <unordered_map>

namespace HephCommon
{
	class UserEventArgs final
	{
	private:
		std::unordered_map<std::string, void*> argsMap;
	public:
		void* operator[](const std::string& key) const;
		size_t Size() const;
		bool Exists(const std::string& key) const;
		void Add(const std::string& key, void* pUserArg);
		void Remove(const std::string& key);
		void Clear();
	};
}