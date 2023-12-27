#include "BlackmanWindow.h"
#include "../HephCommon/HeaderFiles/HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	BlackmanWindow::BlackmanWindow() : BlackmanWindow(0) {}
	BlackmanWindow::BlackmanWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float BlackmanWindow::operator[](size_t n) const 
	{
		return 0.42 - 0.5 * cos(2.0 * Math::pi * n / this->N) + 0.08 * cos(4.0 * Math::pi * n / this->N);
	}
	void BlackmanWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}