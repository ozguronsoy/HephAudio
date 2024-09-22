#include "Windows/HammingWindow.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	HammingWindow::HammingWindow() : HammingWindow(0) {}
	HammingWindow::HammingWindow(size_t size) : Window(size) { this->SetSize(size); }
	double HammingWindow::operator[](size_t n) const 
	{
		return 0.54 - 0.46 * cos(2.0 * HEPH_MATH_PI * n / this->N);
	}
	void HammingWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}