#include "Windows/HannWindow.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	HannWindow::HannWindow() : HannWindow(0) {}
	HannWindow::HannWindow(size_t size) : Window(size) { this->SetSize(size); }
	double HannWindow::operator[](size_t n) const 
	{
		return pow(sin(HEPH_MATH_PI * n / this->N), 2);
	}
	void HannWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}