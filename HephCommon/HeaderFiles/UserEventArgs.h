#pragma once
#include "HephCommonFramework.h"
#include "StringBuffer.h"
#include <unordered_map>

namespace HephCommon
{
	struct UserEventArgsHash final
	{
		size_t operator()(const StringBuffer& lhs) const
		{
			return lhs.Size();
		}
		bool operator()(const StringBuffer& lhs, const StringBuffer& rhs) const
		{
			return lhs.CompareContent(rhs);
		}
	};
	class UserEventArgs final
	{
	private:
		std::unordered_map<StringBuffer, void*, UserEventArgsHash> argsMap;
	public:
		void* operator[](const StringBuffer& key) const;
		size_t Size() const;
		bool Exists(const StringBuffer& key) const;
		void Add(const StringBuffer& key, void* pUserArg);
		void Remove(const StringBuffer& key);
		void Clear();
	};
}