#ifndef _SFX_AUDIOMANAGER_H_
#define _SFX_AUDIOMANAGER_H_

#include <agnostic\resource_manager.h>
#include <SFML\Audio\SoundBuffer.hpp>
#include <SFML\Audio\Music.hpp>
#include <agnostic\logger.h>
using agn::Log;

namespace sfx
{
	class SoundBuffer : public sf::SoundBuffer
	{
	public:
		SoundBuffer(const std::string& _file_name)
		{
			if (loadFromFile(_file_name))
			{
				std::string msg;
				msg.append(_file_name);
				msg.append(" loaded succesfully.");
				Log::Message(msg);
			}
			else
			{
				std::string error;
				error.append(_file_name);
				error.append(" failed to load from file in Sound constructor.");
				Log::Error(error);
			}
		}
	};
	typedef std::unique_ptr<SoundBuffer> UniqueSoundPtr;
	typedef std::shared_ptr<SoundBuffer> SoundPtr;
	typedef std::vector<std::pair<SoundBuffer&, const std::string>> SoundFileVector;

	class Music : public sf::Music
	{
	public:
		Music(const std::string& _file_name)
		{
			if (openFromFile(_file_name))
			{
				std::string msg;
				msg.append(_file_name);
				msg.append(" opened succesfully.");
				Log::Message(msg);
			}
			else
			{
				std::string error;
				error.append(_file_name);
				error.append(" failed to open from file in Music constructor.");
				Log::Error(error);
			}
		}
	};
	typedef std::unique_ptr<Music> UniqueMusicPtr;
	typedef std::shared_ptr<Music> MusicPtr;

	class AudioManager
	{
	public:

		// LoadMusic
		// IN:		String key - file name of music to load
		// OUT:		Shared pointer to the music object
		std::shared_ptr<Music> LoadMusic(const std::string& _file_name)
		{
			return music_manager_.Load(_file_name);
		}

		// LoadSound
		// IN:		String key - file name of sound to load
		// OUT:		Shared pointer to the sound buffer
		std::shared_ptr<SoundBuffer> LoadSound(const std::string& _file_name)
		{
			// Construct a sound from the Sound buffer pointer and return it.
			return sound_manager_.Load(_file_name);
		}

		// UnloadMusic
		// IN:		String key - file name of music to unload
		void UnloadMusic(const std::string& _file_name)
		{
			music_manager_.Unload(_file_name);
		}

		// UnloadSound
		// IN:		String key - file name of sound to unload
		void UnloadSound(const std::string& _file_name)
		{
			sound_manager_.Unload(_file_name);
		}

		// UnloadAllAudio
		// Clears both the sound and music caches
		void UnloadAllAudio()
		{
			sound_manager_.Clear();
			music_manager_.Clear();
		}

	private:
		agn::ResourceManager <std::string, SoundBuffer> sound_manager_;
		agn::ResourceManager <std::string, Music> music_manager_;
	};

}

#endif // _SFX_AUDIOMANAGER_H_