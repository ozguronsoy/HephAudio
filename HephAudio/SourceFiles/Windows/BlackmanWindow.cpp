#include "Windows/BlackmanWindow.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	BlackmanWindow::BlackmanWindow() : BlackmanWindow(0) {}
	BlackmanWindow::BlackmanWindow(size_t size) : Window(size) { this->SetSize(size); }
	double BlackmanWindow::operator[](size_t n) const 
	{
		return 0.42 - 0.5 * cos(2.0 * HEPH_MATH_PI * n / this->N) + 0.08 * cos(4.0 * HEPH_MATH_PI * n / this->N);
	}
	void BlackmanWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}