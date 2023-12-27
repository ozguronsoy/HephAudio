#include "ParzenWindow.h"

namespace HephAudio
{
	ParzenWindow::ParzenWindow() : ParzenWindow(0) {}
	ParzenWindow::ParzenWindow(size_t size) : Window(size) { this->SetSize(size); }
	heph_float ParzenWindow::operator[](size_t n) const 
	{
		const heph_float i = n - this->hN;
		const heph_float absI = fabs(i);
		if (absI >= 0 && absI <= this->qL)
		{
			return 1.0 - 6.0 * pow(i / this->hL, 2) * (1.0 - absI / this->hL);
		}
		return 2.0 * pow(1.0 - absI / this->hL, 3);
	}
	void ParzenWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->hN = 0.5 * (this->size - 1);
		this->hL = this->hN + 0.5;
		this->qL = this->hL * 0.5;
	}
}