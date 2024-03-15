#include "Windows/WelchWindow.h"

namespace HephAudio
{
	WelchWindow::WelchWindow() : WelchWindow(0) {}
	WelchWindow::WelchWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float WelchWindow::operator[](size_t n) const
	{
		return  1.0 - pow((n - this->hN) / this->hN, 2);
	}
	void WelchWindow::SetSize(size_t newSize)
	{
		this->size = newSize;
		this->hN = 0.5 * (this->size - 1);
	}
}