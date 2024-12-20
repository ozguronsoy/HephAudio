#include "Windows/TukeyWindow.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	TukeyWindow::TukeyWindow() : TukeyWindow(0) {}
	TukeyWindow::TukeyWindow(size_t size) : Window(size), alpha(0) { this->SetSize(size); this->SetAlpha(0); }
	TukeyWindow::TukeyWindow(size_t size, double alpha) : Window(size), alpha(alpha) { this->SetSize(size); this->SetAlpha(alpha); }
	double TukeyWindow::operator[](size_t n) const 
	{
		if (n < this->haN)
		{
			return 0.5 * (1.0 - cos(2.0 * HEPH_MATH_PI * n / this->aN));
		}
		else if (n > this->hN)
		{
			return 0.5 * (1.0 - cos(2.0 * HEPH_MATH_PI * (this->N - n) / this->aN));
		}
		return 1.0;
	}
	void TukeyWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
		this->hN = 0.5 * this->N;
		this->aN = this->alpha * this->N;
		this->haN = 0.5 * this->aN;
	}
	double TukeyWindow::GetAlpha() const 
	{
		return this->alpha;
	}
	void TukeyWindow::SetAlpha(double alpha) 
	{
		this->alpha = alpha;
		this->aN = this->alpha * this->N;
		this->haN = 0.5 * this->aN;
	}
}