#include "NuttallWindow.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	NuttallWindow::NuttallWindow() : NuttallWindow(0) {}
	NuttallWindow::NuttallWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float NuttallWindow::operator[](size_t n) const noexcept
	{
		return 0.355768 - 0.487396 * cos(2.0 * Math::pi * n / this->N) + 0.144232 * cos(4.0 * Math::pi * n / this->N) - 0.012604 * cos(6.0 * Math::pi * n / this->N);
	}
	void NuttallWindow::SetSize(size_t newSize) noexcept
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}