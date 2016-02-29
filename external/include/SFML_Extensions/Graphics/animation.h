#ifndef _SFX_ANIMATION_H_
#define _SFX_ANIMATION_H_

#include <agnostic\sprite_animation.h>
#include <SFML\Graphics\Rect.hpp>

namespace sfx
{
	class Animation : public agn::SpriteAnimation
	{
	public:

		// Default arguments - must be given a texture to be valid
		using agn::SpriteAnimation::SpriteAnimation;
		virtual ~Animation() {}

		sf::IntRect GetFrame() const;
		
	};
}

#endif // _SFX_ANIMATION_H_