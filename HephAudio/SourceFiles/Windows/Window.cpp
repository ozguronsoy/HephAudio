#include "Windows/Window.h"

using namespace HephCommon;

namespace HephAudio
{
	Window::Window() : size(0) {}
	Window::Window(size_t size) : size(size) {}
	DoubleBuffer Window::GenerateBuffer() const
	{
		DoubleBuffer buffer(this->size, BufferFlags::AllocUninitialized);
		for (size_t i = 0; i < this->size; ++i)
		{
			buffer[i] = (*this)[i];
		}
		return buffer;
	}
	size_t Window::GetSize() const
	{
		return this->size;
	}
	void Window::SetSize(size_t newSize)
	{
		this->size = newSize;
	}
}