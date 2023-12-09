#include "HannPoissonWindow.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	HannPoissonWindow::HannPoissonWindow() : HannPoissonWindow(0) {}
	HannPoissonWindow::HannPoissonWindow(size_t size) : Window(size), alpha(0) { this->SetSize(size); }
	HannPoissonWindow::HannPoissonWindow(size_t size, heph_float alpha) : Window(size), alpha(alpha) { this->SetSize(size); }
	heph_float HannPoissonWindow::operator[](size_t n) const noexcept
	{
		return 0.5 * (1.0 - cos(2.0 * Math::pi * n / this->N)) * exp(-this->alpha * fabs(this->N - 2.0 * n) / this->N);
	}
	void HannPoissonWindow::SetSize(size_t newSize) noexcept
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}