#include "Windows/BartlettHannWindow.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	BartlettHannWindow::BartlettHannWindow() : BartlettHannWindow(0) {}
	BartlettHannWindow::BartlettHannWindow(size_t size) : Window(size) { this->SetSize(size); }
	double BartlettHannWindow::operator[](size_t n) const 
	{
		return 0.62 - 0.48 * fabs(n / this->N - 0.5) - 0.38 * cos(2.0 * HEPH_MATH_PI * n / this->N);
	}
	void BartlettHannWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}