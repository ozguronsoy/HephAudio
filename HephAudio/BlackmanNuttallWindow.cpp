#include "BlackmanNuttallWindow.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	BlackmanNuttallWindow::BlackmanNuttallWindow() : BlackmanNuttallWindow(0) {}
	BlackmanNuttallWindow::BlackmanNuttallWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float BlackmanNuttallWindow::operator[](size_t n) const noexcept
	{
		return 0.3635819 - 0.4891775 * cos(2.0 * Math::pi * n / this->N) + 0.1365995 * cos(4.0 * Math::pi * n / this->N) - 0.0106411 * cos(6.0 * Math::pi * n / this->N);
	}
	void BlackmanNuttallWindow::SetSize(size_t newSize) noexcept
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}