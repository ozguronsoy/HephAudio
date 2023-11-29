#pragma once
#include "HephAudioFramework.h"
#include "FloatBuffer.h"

namespace HephAudio
{
	class Window
	{
	protected:
		size_t size;
	public:
		Window();
		Window(size_t size);
		virtual ~Window() = default;
		virtual heph_float operator[](size_t n) const noexcept = 0;
		HephCommon::FloatBuffer GenerateBuffer() const;
		size_t GetSize() const noexcept;
		virtual void SetSize(size_t newSize) noexcept;
	};
}