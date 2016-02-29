#pragma once

// This file is used to include everything for an audio-visual application using
// SFML and SFML Extensions. SFML Extensions is dependent on the Agnostic library.
// This file also serves as a home for global objects.

// Graphics, Window and System included.
#include <SFML\Graphics.hpp>
#include <SFML_Extensions\Window\input_manager.h>
#include <SFML_Extensions\Graphics\texture_manager.h>
#include <SFML_Extensions\Graphics\sprite.h>

// Audio included.
#include <SFML\Audio.hpp>
#include <SFML_Extensions\Audio\audio_manager.h>

namespace sfx
{
	class Global
	{
	public:
		static InputManager Input;
		static TextureManager TextureManager;
		static AudioManager AudioManager;
		static void UnloadGlobalResources();
	};
}

using sfx::Global;

using sf::Keyboard;
using sf::Mouse;

using sfx::Sprite;
using sfx::Texture;
using sfx::UniqueTexturePtr;
using sfx::TexturePtr;
using sfx::TextureFileVector;

using sfx::UniqueMusicPtr;
using sfx::MusicPtr;
using sfx::UniqueSoundPtr;
using sfx::SoundPtr;
using sfx::SoundFileVector;