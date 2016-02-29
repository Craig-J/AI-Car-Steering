#ifndef _SFX_SPRITE_H_
#define _SFX_SPRITE_H_

#include <SFML\Graphics\Sprite.hpp>
#include <SFML_Extensions\Graphics\texture_manager.h>
#include <SFML_Extensions\Graphics\animation.h>

namespace sfx
{
	class Sprite : public sf::Sprite
	{
	public:

		// Enum describing the orientation of mapping the texture to the sprite
		enum class TEXTURE_ORIENTATION { NORMAL, FLIP_X, FLIP_Y, FLIP_XY };

		Sprite(sf::Vector2f initial_position = sf::Vector2f(0.0f, 0.0f), TexturePtr texture = nullptr);
		virtual ~Sprite();

		// SetDimensions
		// In:		Vector describing new dimensions
		void SetDimensions(const sf::Vector2f& dimensions);

		// SetDimensions
		// In:		Sprite to copy dimensions from
		void SetDimensions(const sf::Sprite& sprite);

		// SetOrientation
		// Changes the mapping orientation of the texture e.g.,
		// Normal		Flip_X		Flip_XY		Flip_Y
		// --1--		--1--		-----		-----
		// 111--		--111		--111		111--
		// -----		-----		--1--		--1--
		// IN:		Target orientation
		void SetOrientation(TEXTURE_ORIENTATION);

		// SetAnimation
		// IN:		Pointer to animation to use
		// Note:	Does not start the animation - must StartAnimation()
		void SetAnimation(Animation* animation);
		inline Animation* GetAnimation() const { return animation_; }

		// Animate
		// Updates the animation associated with this sprite
		void Animate(const float delta_time);
		
		// StartAnimation
		// Begins the animation by activating updateability
		void StartAnimation();

		// StopAnimation
		// Stops the animation by deactivating updateability
		void StopAnimation();

	private:

		TEXTURE_ORIENTATION orientation_;

		bool animated_;
		Animation* animation_;

		// Internal helper functions
		TEXTURE_ORIENTATION CalculateCurrentOrientation() const;
		void ApplyOrientation();
		void ApplyAnimationUVs();
	};
}

#endif