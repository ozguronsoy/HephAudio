#include "Windows/SineWindow.h"
#include "HephMath.h"

using namespace Heph;

namespace HephAudio
{
	SineWindow::SineWindow() : SineWindow(0) {}
	SineWindow::SineWindow(size_t size) : Window(size) { this->SetSize(size); }
	double SineWindow::operator[](size_t n) const 
	{
		return  sin(HEPH_MATH_PI * n / this->N);
	}
	void SineWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}