#pragma once
#include "framework.h"
#include <string>
#include <vector>

namespace HephAudio
{
	namespace Structs
	{
		struct HephAudioAPI Category
		{
			std::wstring name;
			double volume;
			Category();
			Category(std::wstring name, double volume);
			virtual ~Category() = default;
			bool operator==(const Category& rhs) const;
			bool operator!=(const Category& rhs) const;
		};
		typedef HephAudioAPI std::vector<Category> Categories;
	}
}