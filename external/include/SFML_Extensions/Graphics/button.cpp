#include "button.h"
#include <SFML\Graphics\RenderTarget.hpp>

namespace sfx
{
	Button::Button(sf::Vector2f _initial_position, bool _active, TexturePtr _active_texture, TexturePtr _inactive_texture) :
		Sprite(_initial_position, _active_texture),
		active_texture_(_active_texture),
		inactive_texture_(_inactive_texture),
		active_(_active)
	{
	}

	void Button::Activate()
	{
		active_ = true;
		setTexture(*active_texture_);
	}

	void Button::Deactivate()
	{
		active_ = false;
		setTexture(*inactive_texture_);
	}

	void Button::Toggle()
	{
		active_ = !active_;
		if (active_)
			setTexture(*active_texture_);
		else
			setTexture(*inactive_texture_);
	}
}