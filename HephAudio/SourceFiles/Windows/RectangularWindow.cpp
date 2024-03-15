#include "Windows/RectangularWindow.h"

namespace HephAudio
{
	RectangularWindow::RectangularWindow() : Window() {}
	RectangularWindow::RectangularWindow(size_t size) : Window(size) {}
	heph_float RectangularWindow::operator[](size_t n) const
	{
		return 1.0;
	}
}