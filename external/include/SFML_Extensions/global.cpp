#include "global.h"

namespace sfx
{
	InputManager Global::Input;
	TextureManager Global::TextureManager;
	AudioManager Global::AudioManager;
	void Global::UnloadGlobalResources()
	{
		TextureManager.UnloadAll();
		AudioManager.UnloadAllAudio();
	}
}