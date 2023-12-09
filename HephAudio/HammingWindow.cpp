#include "HammingWindow.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	HammingWindow::HammingWindow() : HammingWindow(0) {}
	HammingWindow::HammingWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float HammingWindow::operator[](size_t n) const noexcept
	{
		return 0.54 - 0.46 * cos(2.0 * Math::pi * n / this->N);
	}
	void HammingWindow::SetSize(size_t newSize) noexcept
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}