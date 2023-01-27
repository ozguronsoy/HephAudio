#include "Category.h"

namespace HephAudio
{
	Category::Category() : Category(L"", 0.0) {}
	Category::Category(std::wstring name, double volume)
	{
		this->name = name;
		this->volume = volume;
	}
	bool Category::operator==(const Category& rhs) const
	{
		return this->name == rhs.name;
	}
	bool Category::operator!=(const Category& rhs) const
	{
		return this->name != rhs.name;
	}
}