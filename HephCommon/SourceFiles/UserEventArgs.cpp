#include "UserEventArgs.h"

namespace HephCommon
{
	void* UserEventArgs::operator[](const StringBuffer& key) const
	{
		return this->Exists(key) ? this->argsMap.at(key) : nullptr;
	}
	size_t UserEventArgs::Size() const
	{
		return this->argsMap.size();
	}
	bool UserEventArgs::Exists(const StringBuffer& key) const
	{
		return this->argsMap.count(key) > 0;
	}
	void UserEventArgs::Add(const StringBuffer& key, void* pUserArg)
	{
		this->argsMap[key] = pUserArg;
	}
	void UserEventArgs::Remove(const StringBuffer& key)
	{
		this->argsMap.erase(key);
	}
	void UserEventArgs::Clear()
	{
		this->argsMap.clear();
	}
}