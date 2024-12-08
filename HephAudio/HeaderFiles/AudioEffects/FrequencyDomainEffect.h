#pragma once
#include "HephAudioShared.h"
#include "OlaEffect.h"
#include "Buffers/ComplexBuffer.h"
#include <vector>

/** @file */

namespace HephAudio
{
	/**
	 * @brief base class for effects that are computed in the frequency domain.
	 * 
	 */
	class HEPH_API FrequencyDomainEffect : public OlaEffect
	{
	protected:
		/** @copydoc default_constructor */
		FrequencyDomainEffect();

		/** @copydoc OlaEffect(size_t) */
		explicit FrequencyDomainEffect(size_t hopSize);

		/** @copydoc OlaEffect(size_t, const Window&) */
		FrequencyDomainEffect(size_t hopSize, const Window& wnd);

	public:
		/** @copydoc destructor */
		virtual ~FrequencyDomainEffect() = default;

		virtual void SetWindow(const Window& wnd) override;
	};
}