#include "Windows/TriangularWindow.h"

namespace HephAudio
{
	TriangularWindow::TriangularWindow() : TriangularWindow(0) {}
	TriangularWindow::TriangularWindow(size_t size) : Window(size) { this->SetSize(size); }
	double TriangularWindow::operator[](size_t n) const 
	{
		return 1.0 - fabs((n - this->hN) / this->hL);
	}
	void TriangularWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->hN = 0.5 * (this->size - 1);
		this->hL = this->hN + 1.0;
	}
}