#include "Windows/HannPoissonWindow.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	HannPoissonWindow::HannPoissonWindow() : HannPoissonWindow(0) {}
	HannPoissonWindow::HannPoissonWindow(size_t size) : Window(size), alpha(0) { this->SetSize(size); }
	HannPoissonWindow::HannPoissonWindow(size_t size, double alpha) : Window(size), alpha(alpha) { this->SetSize(size); }
	double HannPoissonWindow::operator[](size_t n) const 
	{
		return 0.5 * (1.0 - cos(2.0 * HEPH_MATH_PI * n / this->N)) * exp(-this->alpha * fabs(this->N - 2.0 * n) / this->N);
	}
	void HannPoissonWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}