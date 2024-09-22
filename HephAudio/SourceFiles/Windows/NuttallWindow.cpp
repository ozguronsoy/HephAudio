#include "Windows/NuttallWindow.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	NuttallWindow::NuttallWindow() : NuttallWindow(0) {}
	NuttallWindow::NuttallWindow(size_t size) : Window(size) { this->SetSize(size); }
	double NuttallWindow::operator[](size_t n) const 
	{
		return 0.355768 - 0.487396 * cos(2.0 * HEPH_MATH_PI * n / this->N) + 0.144232 * cos(4.0 * HEPH_MATH_PI * n / this->N) - 0.012604 * cos(6.0 * HEPH_MATH_PI * n / this->N);
	}
	void NuttallWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}