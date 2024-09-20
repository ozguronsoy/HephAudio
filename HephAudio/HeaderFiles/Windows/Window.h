#pragma once
#include "HephAudioShared.h"
#include "Buffers/DoubleBuffer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for windows.
	 * 
	 */
	class HEPH_API Window
	{
	protected:
		/**
		 * size of the window
		 * 
		 */
		size_t size;

	protected:
		/** @copydoc default_constructor */
		Window();

		/** 
		 * @copydoc constructor 
		 * 
		 * @param size @copydetails size
		 */
		Window(size_t size);

	public:
		/** @copydoc destructor */
		virtual ~Window() = default;

		/**
		 * calculates the sample at index n.
		 * 
		 */
		virtual double operator[](size_t n) const = 0;

		/**
		 * generates a buffer.
		 * 
		 */
		HephCommon::DoubleBuffer GenerateBuffer() const;

		/**
		 * gets the size of the window.
		 * 
		 */
		size_t GetSize() const;

		/**
		 * sets the size of the window.
		 * 
		 */
		virtual void SetSize(size_t newSize);
	};
}