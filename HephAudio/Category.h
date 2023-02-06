#pragma once
#include "framework.h"
#include "StringBuffer.h"
#include <vector>

namespace HephAudio
{
	struct Category
	{
		StringBuffer name;
		double volume;
		Category();
		Category(StringBuffer name, double volume);
		virtual ~Category() = default;
		bool operator==(const Category& rhs) const;
		bool operator!=(const Category& rhs) const;
	};
	typedef std::vector<Category> Categories;
}