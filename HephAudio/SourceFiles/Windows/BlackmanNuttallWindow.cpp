#include "Windows/BlackmanNuttallWindow.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	BlackmanNuttallWindow::BlackmanNuttallWindow() : BlackmanNuttallWindow(0) {}
	BlackmanNuttallWindow::BlackmanNuttallWindow(size_t size) : Window(size) { this->SetSize(size); }
	double BlackmanNuttallWindow::operator[](size_t n) const 
	{
		return 0.3635819 - 0.4891775 * cos(2.0 * HEPH_MATH_PI * n / this->N) + 0.1365995 * cos(4.0 * HEPH_MATH_PI * n / this->N) - 0.0106411 * cos(6.0 * HEPH_MATH_PI * n / this->N);
	}
	void BlackmanNuttallWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}