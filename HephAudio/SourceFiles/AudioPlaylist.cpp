#include "AudioPlaylist.h"
#include "ConsoleLogger.h"
#include "Exceptions/InvalidArgumentException.h"

using namespace Heph;

namespace HephAudio
{
	AudioPlaylist::AudioPlaylist(Native::NativeAudio* pNativeAudio) : AudioPlaylist(pNativeAudio, {}) {}

	AudioPlaylist::AudioPlaylist(Audio& audio) : AudioPlaylist(audio.GetNativeAudio()) {}

	AudioPlaylist::AudioPlaylist(Native::NativeAudio* pNativeAudio, const std::vector<std::filesystem::path>& files)
		: stream(pNativeAudio), files(files)
	{
		this->ChangeFile();
	}

	AudioPlaylist::AudioPlaylist(Audio& audio, const std::vector<std::filesystem::path>& files)
		: AudioPlaylist(audio.GetNativeAudio(), files) {}

	AudioPlaylist::AudioPlaylist(AudioPlaylist&& rhs) noexcept
		: stream(std::move(rhs.stream)), files(std::move(rhs.files)) {}

	AudioPlaylist& AudioPlaylist::operator=(const std::filesystem::path& rhs)
	{
		this->Clear();
		this->Add(rhs);
		return *this;
	}

	AudioPlaylist& AudioPlaylist::operator=(const std::vector<std::filesystem::path>& rhs)
	{
		this->Clear();
		this->Add(rhs);
		return *this;
	}

	AudioPlaylist& AudioPlaylist::operator=(AudioPlaylist&& rhs) noexcept
	{
		if (this != &rhs)
		{
			this->stream = std::move(rhs.stream);
			this->files = std::move(rhs.files);
		}

		return *this;
	}

	size_t AudioPlaylist::Size() const
	{
		return this->files.size();
	}

	Native::NativeAudio* AudioPlaylist::GetNativeAudio() const
	{
		return this->stream.GetNativeAudio();
	}

	void AudioPlaylist::Start()
	{
		if (this->files.size() > 0)
		{
			this->stream.GetAudioObject()->isPaused = false;
		}
	}

	void AudioPlaylist::Stop()
	{
		if (this->files.size() > 0)
		{
			this->stream.GetAudioObject()->isPaused = true;
		}
	}

	void AudioPlaylist::Add(const std::filesystem::path& filePath)
	{
		this->files.push_back(filePath);
		if (this->files.size() == 1)
		{
			this->ChangeFile();
		}
	}

	void AudioPlaylist::Add(const std::vector<std::filesystem::path>& files)
	{
		if (files.size() > 0)
		{
			this->files.insert(this->files.end(), files.begin(), files.end());
			if (this->files.size() == files.size())
			{
				this->ChangeFile();
			}
		}
	}

	void AudioPlaylist::Insert(const std::filesystem::path& filePath, size_t index)
	{
		if (index > this->files.size())
		{
			HEPH_RAISE_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Index out of range."));
			index = this->files.size();
		}

		this->files.insert(this->files.begin() + index, filePath);
		if (index == 0)
		{
			this->ChangeFile();
		}
	}

	void AudioPlaylist::Insert(const std::vector<std::filesystem::path>& files, size_t index)
	{
		if (files.size() > 0)
		{
			if (index > this->files.size())
			{
				HEPH_RAISE_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Index out of range."));
				index = this->files.size();
			}

			this->files.insert(this->files.begin() + index, files.begin(), files.end());
			if (index == 0)
			{
				this->ChangeFile();
			}
		}
	}

	void AudioPlaylist::Remove(size_t index)
	{
		if (index >= this->files.size())
		{
			HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Index out of range."));
		}

		this->files.erase(this->files.begin() + index);
		if (index == 0)
		{
			this->ChangeFile();
		}
	}

	void AudioPlaylist::Remove(size_t index, size_t count)
	{
		if (count > 0)
		{
			if (index >= this->files.size())
			{
				HEPH_RAISE_AND_THROW_EXCEPTION(this, InvalidArgumentException(HEPH_FUNC, "Index out of range."));
			}

			if (index + count >= this->files.size())
			{
				count = index - this->files.size();
			}

			this->files.erase(this->files.begin() + index, this->files.begin() + index + count);
			if (index == 0)
			{
				this->ChangeFile();
			}
		}
	}

	void AudioPlaylist::Remove(const std::filesystem::path& filePath)
	{
		for (size_t i = 0; i < this->files.size(); i++)
		{
			if (this->files[i] == filePath)
			{
				this->Remove(i);
				return;
			}
		}
	}

	void AudioPlaylist::Skip()
	{
		this->Skip(1);
	}

	void AudioPlaylist::Skip(size_t n)
	{
		if (n > 0)
		{
			if (n >= this->files.size())
			{
				this->Clear();
				return;
			}
			this->files.erase(this->files.begin(), this->files.begin() + n);
			this->ChangeFile();
		}
	}

	void AudioPlaylist::Clear()
	{
		this->files.clear();
		Native::NativeAudio* pNativeAudio = this->stream.GetNativeAudio();
		this->stream.Release();
		this->stream = AudioStream(pNativeAudio, "");
	}

	void AudioPlaylist::ChangeFile()
	{
		if (this->files.size() == 0)
		{
			this->stream.ChangeFile("");
			return;
		}

		std::filesystem::path filePath = this->files[0];
		AudioObject* pAudioObject = this->stream.GetAudioObject();

	STREAM_CHANGE_FILE:
		try
		{
			this->stream.ChangeFile(filePath);
			pAudioObject = this->stream.GetAudioObject();

			if (!pAudioObject->OnFinishedPlaying.userEventArgs.Exists(HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY))
			{
				pAudioObject->OnFinishedPlaying = &AudioPlaylist::OnFinishedPlaying;
				pAudioObject->OnRender.userEventArgs.Add(HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY, this);
				pAudioObject->OnFinishedPlaying.userEventArgs.Add(HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY, this);
			}
		}
		catch (const Exception&)
		{
			this->files.erase(this->files.begin());
			if (this->files.size() > 0)
			{
				HEPHAUDIO_LOG("Could not play \"" + filePath.string() + "\", trying next file...", HEPH_CL_WARNING);
				filePath = this->files[0];
				goto STREAM_CHANGE_FILE;
			}
			else
			{
				HEPHAUDIO_LOG("Could not play \"" + filePath.string() + "\", playlist finished.", HEPH_CL_WARNING);
				this->stream.ChangeFile("");
			}
		}
	}

	void AudioPlaylist::OnFinishedPlaying(const EventParams& eventParams)
	{
		AudioFinishedPlayingEventArgs* pFinishedPlayingEventArgs = (AudioFinishedPlayingEventArgs*)eventParams.pArgs;
		AudioPlaylist* pPlaylist = (AudioPlaylist*)eventParams.userEventArgs[HEPHAUDIO_PLAYLIST_EVENT_USER_ARG_KEY];

		if (pPlaylist != nullptr)
		{
			pPlaylist->Remove(0);
			eventParams.pResult->isHandled = true;
		}
	}
}