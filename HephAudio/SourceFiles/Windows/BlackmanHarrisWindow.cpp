#include "Windows/BlackmanHarrisWindow.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	BlackmanHarrisWindow::BlackmanHarrisWindow() : BlackmanHarrisWindow(0) {}
	BlackmanHarrisWindow::BlackmanHarrisWindow(size_t size) : Window(size) { this->SetSize(size); }
	double BlackmanHarrisWindow::operator[](size_t n) const 
	{
		return 0.35875 - 0.48829 * cos(2.0 * HEPH_MATH_PI * n / this->N) + 0.14128 * cos(4.0 * HEPH_MATH_PI * n / this->N) - 0.01168 * cos(6.0 * HEPH_MATH_PI * n / this->N);
	}
	void BlackmanHarrisWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}