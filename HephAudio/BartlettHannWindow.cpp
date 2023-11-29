#include "BartlettHannWindow.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	BartlettHannWindow::BartlettHannWindow() : BartlettHannWindow(0) {}
	BartlettHannWindow::BartlettHannWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float BartlettHannWindow::operator[](size_t n) const noexcept
	{
		return 0.62 - 0.48 * fabs(n / this->N - 0.5) - 0.38 * cos(2.0 * Math::pi * n / this->N);
	}
	void BartlettHannWindow::SetSize(size_t newSize) noexcept
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}