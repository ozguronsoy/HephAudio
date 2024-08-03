#include "Windows/GaussianWindow.h"

namespace HephAudio
{
	GaussianWindow::GaussianWindow() : GaussianWindow(0) {}
	GaussianWindow::GaussianWindow(size_t size) : Window(size), sigma(0) { this->SetSize(size); this->SetSigma(0); }
	GaussianWindow::GaussianWindow(size_t size, double sigma) : Window(size), sigma(sigma) { this->SetSize(size); this->SetSigma(sigma); }
	double GaussianWindow::operator[](size_t n) const 
	{
		return exp(-0.5 * pow((n - this->hN) / this->shN, 2));
	}
	void GaussianWindow::SetSize(size_t newSize) 
	{
		this->size = newSize;
		this->hN = 0.5 * (this->size - 1);
		this->shN = this->sigma * this->hN;
	}
	double GaussianWindow::GetSigma() const 
	{
		return this->sigma;
	}
	void GaussianWindow::SetSigma(double sigma) 
	{
		this->sigma = sigma;
		this->shN = this->sigma * this->hN;
	}
}