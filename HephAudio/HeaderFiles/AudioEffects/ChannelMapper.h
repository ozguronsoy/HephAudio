#pragma once
#include "DoubleBufferedAudioEffect.h"
#include <unordered_map>
#include <functional>

/** @file */

namespace HephAudio
{
	/**
	 * @brief remaps the audio channels.
	 *
	 */
	class HEPH_API ChannelMapper : public DoubleBufferedAudioEffect
	{
	protected:
		struct AudioChannelLayoutHasher
		{
			size_t operator()(const AudioChannelLayout& chLayout) const;
		};

		/**
		 * layout-(layout-function) map. \n
		 * 
		 * @par Example
		 * from mono to stereo would look like stereo-(mono-function) where function would do the conversion.
		 *
		 */
		typedef std::unordered_map<
			AudioChannelLayout,
			std::unordered_map<AudioChannelLayout, std::function<void(heph_audio_sample_t*, heph_audio_sample_t*)>, AudioChannelLayoutHasher>,
			AudioChannelLayoutHasher> ChannelMapperMap;

	protected:
		/**
		 * contains the functions to map the channel layouts.
		 *
		 */
		static ChannelMapperMap map;

	protected:
		/**
		 * channel layout the input buffer will be remapped to.
		 *
		 */
		AudioChannelLayout targetLayout;

	public:
		/** @copydoc default_constructor */
		ChannelMapper();

		/**
		 * @copydoc constructor
		 *
		 * @param targetLayout @copydetails targetLayout
		 *
		 */
		explicit ChannelMapper(const AudioChannelLayout& targetLayout);

		/** @copydoc destructor */
		virtual ~ChannelMapper() = default;

		virtual std::string Name() const override;

		/**
		 * gets the target channel layout.
		 *
		 */
		virtual const AudioChannelLayout& GetTargetLayout() const;

		/**
		 * sets the target channel layout.
		 *
		 * @param targetLayout @copydetails targetLayout
		 *
		 */
		virtual void SetTargetLayout(const AudioChannelLayout& targetLayout);

	protected:
		virtual void ProcessST(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) override;
		virtual AudioBuffer CreateOutputBuffer(const AudioBuffer& inputBuffer, size_t startIndex, size_t frameCount) const override;
		virtual void InitializeOutputBuffer(const AudioBuffer& inputBuffer, AudioBuffer& outputBuffer, size_t startIndex, size_t frameCount) const override;

	public:
		/**
		 * sets mapping from targetLayout to inputLayout.
		 * 
		 * @param targetLayout target (output) channel layout.
		 * @param inputLayout input channel layout.
		 * @param f function that maps the channels.
		 * 
		 */
		static void SetMapping(const AudioChannelLayout& targetLayout, const AudioChannelLayout& inputLayout, const std::function<void(heph_audio_sample_t*, heph_audio_sample_t*)>& f);

	protected:
		/**
		 * creates the ChannelMapper::map.
		 *
		 */
		static ChannelMapperMap CreateMap();
	};
}