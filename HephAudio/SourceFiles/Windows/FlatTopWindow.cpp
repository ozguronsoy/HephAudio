#include "Windows/FlatTopWindow.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	FlatTopWindow::FlatTopWindow() : FlatTopWindow(0) {}
	FlatTopWindow::FlatTopWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float FlatTopWindow::operator[](size_t n) const 
	{
		return 0.21557895 - 0.41663158 * cos(2.0 * HEPH_MATH_PI * n / this->N) + 0.277263158 * cos(4.0 * HEPH_MATH_PI * n / this->N) - 0.083578947 * cos(6.0 * HEPH_MATH_PI * n / this->N) + 0.006947368 * cos(8.0 * HEPH_MATH_PI * n / this->N);
	}
	void FlatTopWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}