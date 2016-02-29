#ifndef _SFX_BUTTON_H_
#define _SFX_BUTTON_H_
#include <SFML_Extensions\Graphics\sprite.h>

namespace sfx
{
	class Button : public Sprite
	{
	public:

		Button(sf::Vector2f _initial_position = sf::Vector2f(0.0f, 0.0f), bool _active = true, TexturePtr _active_texture = nullptr, TexturePtr _inactive_texture = nullptr);
		virtual ~Button() {}

		void Activate();
		void Deactivate();
		void Toggle();

	private:

		bool active_;
		TexturePtr active_texture_;
		TexturePtr inactive_texture_;

	};
}
#endif // _SFX_BUTTON_H_