#include "Windows/LanczosWindow.h"
#include "HephMath.h"

using namespace HephCommon;

namespace HephAudio
{
	LanczosWindow::LanczosWindow() : LanczosWindow(0) {}
	LanczosWindow::LanczosWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float LanczosWindow::operator[](size_t n) const 
	{
		const heph_float pix = HephMath::pi * (2.0 * n / this->N - 1.0);
		return sin(pix) / pix;
	}
	void LanczosWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->N = this->size - 1;
	}
}