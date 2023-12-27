#include "GaussianWindow.h"

namespace HephAudio
{
	GaussianWindow::GaussianWindow() : GaussianWindow(0) {}
	GaussianWindow::GaussianWindow(size_t size) : Window(size), sigma(0) { this->SetSize(size); this->SetSigma(0); }
	GaussianWindow::GaussianWindow(size_t size, heph_float sigma) : Window(size), sigma(sigma) { this->SetSize(size); this->SetSigma(sigma); }
	heph_float GaussianWindow::operator[](size_t n) const 
	{
		return exp(-0.5 * pow((n - this->hN) / this->shN, 2));
	}
	void GaussianWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->hN = 0.5 * (this->size - 1);
		this->shN = this->sigma * this->hN;
	}
	heph_float GaussianWindow::GetSigma() const 
	{
		return this->sigma;
	}
	void GaussianWindow::SetSigma(heph_float sigma) 
	{
		this->sigma = sigma;
		this->shN = this->sigma * this->hN;
	}
}