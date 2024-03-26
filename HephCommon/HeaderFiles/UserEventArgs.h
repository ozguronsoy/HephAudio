#pragma once
#include "HephCommonShared.h"
#include "StringBuffer.h"
#include <unordered_map>

namespace HephCommon
{
	class UserEventArgs final
	{
	private:
		class UserEventArgsHash final
		{
		public:
			size_t operator()(const StringBuffer& lhs) const
			{
				return lhs.Size();
			}
			bool operator()(const StringBuffer& lhs, const StringBuffer& rhs) const
			{
				return lhs.CompareContent(rhs);
			}
		};
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