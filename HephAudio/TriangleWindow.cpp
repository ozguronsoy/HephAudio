#include "TriangleWindow.h"

namespace HephAudio
{
	TriangleWindow::TriangleWindow() : TriangleWindow(0) {}
	TriangleWindow::TriangleWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float TriangleWindow::operator[](size_t n) const 
	{
		return 1.0 - fabs((n - this->hN) / this->hL);
	}
	void TriangleWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->hN = 0.5 * (this->size - 1);
		this->hL = this->hN + 1.0;
	}
}