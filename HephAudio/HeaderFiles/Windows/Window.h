#pragma once
#include "HephAudioShared.h"
#include "FloatBuffer.h"

namespace HephAudio
{
	class Window
	{
	protected:
		size_t size;
	protected:
		Window();
		Window(size_t size);
	public:
		virtual ~Window() = default;
		virtual heph_float operator[](size_t n) const = 0;
		HephCommon::FloatBuffer GenerateBuffer() const;
		size_t GetSize() const;
		virtual void SetSize(size_t newSize);
	};
}