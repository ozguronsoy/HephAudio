#pragma once
#include "framework.h"
#include "StringBuffer.h"
#include <vector>

namespace HephAudio
{
	struct Category
	{
		StringBuffer name;
		HEPHAUDIO_DOUBLE volume;
		Category();
		Category(StringBuffer name, HEPHAUDIO_DOUBLE volume);
		virtual ~Category() = default;
		bool operator==(const Category& rhs) const;
		bool operator!=(const Category& rhs) const;
	};
	typedef std::vector<Category> Categories;
}