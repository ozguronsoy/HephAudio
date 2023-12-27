#include "HannWindow.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	HannWindow::HannWindow() : HannWindow(0) {}
	HannWindow::HannWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float HannWindow::operator[](size_t n) const 
	{
		return pow(sin(Math::pi * n / this->N), 2);
	}
	void HannWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}