#pragma once
#include "HephAudioShared.h"
#include "DoubleBufferedAudioEffect.h"
#include "Windows/Window.h"
#include "Buffers/DoubleBuffer.h"

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for effects that use overlap-add method.
	 * 
	 */
	class OlaEffect : public DoubleBufferedAudioEffect
	{
	protected:
		/**
		 * number of frames to advance each iteration.
		 *
		 */
		size_t hopSize;

		/**
		 * window that will be applied for overlap-add.
		 *
		 */
		Heph::DoubleBuffer wnd;

	protected:
		/** @copydoc default_constructor */
		OlaEffect();

		/**
		 * @copydoc constructor
		 *
		 * @param hopSize @copydetails hopSize
		 *
		 */
		explicit OlaEffect(size_t hopSize);

		/**
		 * @copydoc constructor
		 *
		 * @param hopSize @copydetails hopSize
		 * @param wnd @copydetails wnd
		 *
		 */
		OlaEffect(size_t hopSize, const Window& wnd);

		/**
		 * gets the hop size.
		 *
		 */
		virtual size_t GetHopSize() const;

		/**
		 * sets the hop size.
		 *
		 * @param hopSize @copydetails hopSize
		 *
		 */
		virtual void SetHopSize(size_t hopSize);

		/**
		 * gets the size of the window.
		 *
		 */
		virtual size_t GetWindowSize() const;

		/**
		 * sets the window.
		 *
		 * @param wnd @copydetails wnd.
		 *
		 */
		virtual void SetWindow(const Window& wnd);

	public:
		/** @copydoc destructor */
		virtual ~OlaEffect() = default;

	protected:
		/**
		 * calculates the maximum number of overlaps
		 * 
		 */
		virtual size_t CalculateMaxNumberOfOverlaps() const;
	};
}