#include "Windows/ExactBlackmanWindow.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	ExactBlackmanWindow::ExactBlackmanWindow() : ExactBlackmanWindow(0) {}
	ExactBlackmanWindow::ExactBlackmanWindow(size_t size) : Window(size) { this->SetSize(size); }
	double ExactBlackmanWindow::operator[](size_t n) const 
	{
		return 0.42659 - 0.49656 * cos(2.0 * HEPH_MATH_PI * n / this->N) + 0.076849 * cos(4.0 * HEPH_MATH_PI * n / this->N);
	}
	void ExactBlackmanWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}