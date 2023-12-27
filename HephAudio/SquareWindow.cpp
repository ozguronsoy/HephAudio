#include "SquareWindow.h"

namespace HephAudio
{
	SquareWindow::SquareWindow() : Window() {}
	SquareWindow::SquareWindow(size_t size) : Window(size) {}
	heph_float SquareWindow::operator[](size_t n) const 
	{
		return 1.0;
	}
}