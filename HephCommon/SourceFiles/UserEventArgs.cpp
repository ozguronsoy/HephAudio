#include "UserEventArgs.h"

namespace HephCommon
{
	void* UserEventArgs::operator[](const std::string& key) const
	{
		return this->Exists(key) ? this->argsMap.at(key) : nullptr;
	}
	size_t UserEventArgs::Size() const
	{
		return this->argsMap.size();
	}
	bool UserEventArgs::Exists(const std::string& key) const
	{
		return this->argsMap.count(key) > 0;
	}
	void UserEventArgs::Add(const std::string& key, void* pUserArg)
	{
		this->argsMap[key] = pUserArg;
	}
	void UserEventArgs::Remove(const std::string& key)
	{
		this->argsMap.erase(key);
	}
	void UserEventArgs::Clear()
	{
		this->argsMap.clear();
	}
}