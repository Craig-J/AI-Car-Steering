#include "sprite.h"
#include <agnostic\logger.h>

namespace sfx
{
	Sprite::Sprite(sf::Vector2f _initial_position, TexturePtr _texture) :
		sf::Sprite(),
		animated_(false),
		animation_(nullptr),
		orientation_(TEXTURE_ORIENTATION::NORMAL)
	{
		if (_texture) setTexture(*_texture, true);
		setOrigin(getLocalBounds().width / 2.0f, getLocalBounds().height / 2.0f);
		setPosition(_initial_position);
	}

	Sprite::~Sprite()
	{
	}

	void Sprite::SetDimensions(const sf::Vector2f& _dimensions)
	{
		sf::FloatRect current_dimensions = getLocalBounds(); // current sprite dimensions
		sf::Vector2f scalefactor(_dimensions.x / current_dimensions.width, _dimensions.y / current_dimensions.height); // create scalefactor from new dimensions/old dimensions
		setScale(scalefactor);
		setOrigin(getLocalBounds().width / 2.0f, getLocalBounds().height / 2.0f);
	}

	void Sprite::SetDimensions(const sf::Sprite& _sprite)
	{
		sf::FloatRect current_dimensions = getLocalBounds(); // current sprite dimensions
		sf::FloatRect new_dimensions = _sprite.getLocalBounds(); // new sprite dimensions
		sf::Vector2f scalefactor(new_dimensions.width / current_dimensions.width, new_dimensions.height / current_dimensions.height);
		setScale(scalefactor);
		setOrigin(getLocalBounds().width / 2.0f, getLocalBounds().height / 2.0f);
	}

	void Sprite::SetOrientation(TEXTURE_ORIENTATION _orientation)
	{
		orientation_ = _orientation;
		ApplyOrientation();
	}

	void Sprite::SetAnimation(Animation* _animation)
	{
		animation_ = _animation;
	}

	void Sprite::Animate(const float _delta_time)
	{
		if (animated_ && animation_)
		{
			animation_->Update(_delta_time);
			
		}
	}

	void Sprite::StartAnimation()
	{
		if (animation_)
		{
			animated_ = true;
			animation_->ActivateUpdate();
		}
	}

	void Sprite::StopAnimation()
	{
		if (animation_)
		{
			animated_ = false;
			animation_->DeactivateUpdate();
		}
	}

	Sprite::TEXTURE_ORIENTATION Sprite::CalculateCurrentOrientation() const
	{
		// Get current rectangle the sprite displays
		auto rect = getTextureRect();

		// Check sign on width and height to determine orientation
		if (rect.width > 0)
		{
			if (rect.height > 0)
				return TEXTURE_ORIENTATION::NORMAL;		// Both X and Y positive
			else if (rect.height < 0)
				return TEXTURE_ORIENTATION::FLIP_Y;		// X positive, Y negative
		}
		else if (rect.width < 0)
		{
			if (rect.height > 0)
				return TEXTURE_ORIENTATION::FLIP_X;		// X negative, Y positive
			else if (rect.height < 0)
				return TEXTURE_ORIENTATION::FLIP_XY;	// Both X and Y negative
		}
		agn::Log::Error("CalculateCurrentOrientation failed - invalid values.");
		return orientation_;
	}

	void Sprite::ApplyOrientation()
	{
		// Get the current rectangle the sprite displays
		auto rect = getTextureRect();

		// Holds the operation to perform
		TEXTURE_ORIENTATION operation = TEXTURE_ORIENTATION::NORMAL; // Do nothing by default

		// Nested switch seems unavoidable here
		switch (CalculateCurrentOrientation())
		{
		case TEXTURE_ORIENTATION::NORMAL:	// Undo current state by applying the same operation
			switch (orientation_)
			{
			case TEXTURE_ORIENTATION::FLIP_X:
				operation = TEXTURE_ORIENTATION::FLIP_X;
				break;
			case TEXTURE_ORIENTATION::FLIP_XY:
				operation = TEXTURE_ORIENTATION::FLIP_XY;
				break;
			case TEXTURE_ORIENTATION::FLIP_Y:
				operation = TEXTURE_ORIENTATION::FLIP_Y;
				break;
			}
			break;
		case TEXTURE_ORIENTATION::FLIP_X:
			switch (orientation_)
			{
			case TEXTURE_ORIENTATION::NORMAL:
				operation = TEXTURE_ORIENTATION::FLIP_X;	// Simply apply Flip_X
				break;
			case TEXTURE_ORIENTATION::FLIP_XY:
				operation = TEXTURE_ORIENTATION::FLIP_Y;	// Undo Flip_Y
				break;
			case TEXTURE_ORIENTATION::FLIP_Y:
				operation = TEXTURE_ORIENTATION::FLIP_XY;	// Apply Flip_X and undo Flip_Y
				break;
			}
			break;
		case TEXTURE_ORIENTATION::FLIP_XY:
			switch (orientation_)
			{
			case TEXTURE_ORIENTATION::NORMAL:
				operation = TEXTURE_ORIENTATION::FLIP_XY;	// Simply apply Flip_XY
				break;
			case TEXTURE_ORIENTATION::FLIP_X:
				operation = TEXTURE_ORIENTATION::FLIP_Y;	// Apply Flip_Y
				break;
			case TEXTURE_ORIENTATION::FLIP_Y:
				operation = TEXTURE_ORIENTATION::FLIP_X;	// Apply Flip_X
				break;
			}
			break;
		case TEXTURE_ORIENTATION::FLIP_Y:
			switch (orientation_)
			{
			case TEXTURE_ORIENTATION::NORMAL:
				operation = TEXTURE_ORIENTATION::FLIP_Y;	// Simply apply Flip_Y
				break;
			case TEXTURE_ORIENTATION::FLIP_X:
				operation = TEXTURE_ORIENTATION::FLIP_XY;	// Apply Flip_X and undo Flip_Y
				break;
			case TEXTURE_ORIENTATION::FLIP_XY:
				operation = TEXTURE_ORIENTATION::FLIP_X;	// Undo Flip_X
				break;
			}
			break;
		}

		switch (operation)
		{
		case TEXTURE_ORIENTATION::NORMAL:
			// NORMAL - do nothing, no operation required
			break;
		case TEXTURE_ORIENTATION::FLIP_X:
			setTextureRect(sf::IntRect(rect.left + rect.width, rect.top, -rect.width, rect.height));
			break;
		case TEXTURE_ORIENTATION::FLIP_XY:
			setTextureRect(sf::IntRect(rect.left + rect.width, rect.top + rect.height, -rect.width, -rect.height));
			break;
		case TEXTURE_ORIENTATION::FLIP_Y:
			setTextureRect(sf::IntRect(rect.left, rect.top + rect.height, rect.width, -rect.height));
			break;
		}
	}
}