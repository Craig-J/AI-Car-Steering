#include "sprite_animation.h"
#include "logger.h"

namespace agn
{
	SpriteAnimation::SpriteAnimation(const int _texture_width,
		const int _texture_height,
		const float _frame_duration,
		const int _frame_count,
		const int _current_frame,
		const bool _looping) :
		finished_(false),
		sheet_type_(SPRITESHEET_TYPE::SCROLL_X),
		texture_width_(_texture_width),
		texture_height_(_texture_height),
		start_frame_(_current_frame),
		end_frame_(_frame_count)
	{
		SetUpdateFrameTime(_frame_duration);
		SetFrameCount(_frame_count, _frame_count);
		SetLooping(_looping);
	}

	void SpriteAnimation::ResetAnimation()
	{
		// Reset to initial variables
		current_frame_ = start_frame_;
		current_loop_ = 0;
		finished_ = false;
	}

	void SpriteAnimation::SetSpriteSheetType(const SPRITESHEET_TYPE _type)
	{
		sheet_type_ = _type;
	}

	void SpriteAnimation::SetFrameCount(const int _total, const int _x_count, const int _y_count)
	{
		animation_frame_count_ = _total;

		x_frame_count_ = _x_count;
		y_frame_count_ = _y_count;

		CalculateUVDimensions();
	}

	void SpriteAnimation::SetLooping(const bool _looping, const int _number_of_loops)
	{
		looping_ = _looping;
		current_loop_ = 0;
		loop_count_ = _number_of_loops;
	}

	void SpriteAnimation::SetOffset(const int _start_frame, const int _end_frame)
	{
		start_frame_ = _start_frame;
		end_frame_ = _end_frame;

		if (start_frame_ > animation_frame_count_ || start_frame_ <= 0)
		{
			Log::Warning("Animation start frame is invalid.");
			Log::Warning("    If nonlinear - animation may behave unexpectedly.");
			Log::Warning("    If linear - animation start frame is now first frame.");

			// If we are using a linear sheet type (if it's not nonlinear)
			/*if (sheet_type_ != SPRITESHEET_TYPE::NONLINEAR)
			{
				// Set start frame to first frame in animation to limit the damage.
				start_frame_ = 1;
			}*/
		}

		if (end_frame_ > animation_frame_count_ || end_frame_ <= 0)
		{
			Log::Warning("Animation end frame is invalid.");
			Log::Warning("    If nonlinear - animation will behave unexpectedly and never end.");
			Log::Warning("    If linear - animation end frame is now last frame.");

			// If we are using a linear sheet type (if it's not nonlinear)
			/*if (sheet_type_ != SPRITESHEET_TYPE::NONLINEAR)
			{
				// Set end frame to last frame in animation to limit the damage.
				end_frame_ = animation_frame_count_;
			}*/
		}
	}

	void SpriteAnimation::OnUpdate(const float delta_time)
	{
		// Delta time unused in this scope as animations play in discrete frames

		if (current_frame_ == start_frame_ && current_loop_ == 0)
		{
			OnAnimationStart();
		}
		if (current_frame_ == end_frame_)
		{
			OnAnimationEnd();
			if (looping_)
			{
				if (current_loop_ == loop_count_) // if this was the last loop
				{
					// Animation is finished
					OnAnimationEnd();
					DeactivateUpdate();
					return;
				}
				else
				{
					// Animation is looping
					OnAnimationLoop();
					current_loop_++;
					current_frame_ = start_frame_;
				}
			}
			else
			{
				// Animation is finished
				OnAnimationEnd();
				DeactivateUpdate();
				return;
			}
		}

		// No nonlinear case here since current_frame will be used directly
		// and UVs are fetched from a container.
		switch (sheet_type_)
		{
		case SPRITESHEET_TYPE::SCROLL_X: current_x_frame_ = current_frame_;
			break;
		case SPRITESHEET_TYPE::SCROLL_Y: current_y_frame_ = current_frame_;
			break;


			// TO DO: Find way to calculate start frame and x/y frames
			/*case SPRITESHEET_TYPE::SCROLL_XY:

			current_x_frame_ = (current_frame_%x_frame_count_) + 1;
			current_y_frame_ = (current_frame_%y_frame_count_) + 1;

			if (current_x_frame_ > x_frame_count_)
			{
			current_y_frame_++;
			}
			else
			{
			current_x_frame_++;
			}
			break;
			case SPRITESHEET_TYPE::SCROLL_YX:
			if (current_y_frame_ > y_frame_count_)
			{
			current_x_frame_++;
			}
			else
			{
			current_y_frame_++;
			}
			break;*/
		}

		// If we are using a linear sheet type
		/*if (sheet_type_ == SPRITESHEET_TYPE::NONLINEAR)
		{
			// TO-DO: Fetch from a structure of frame data here
			
		}
		else
		{*/
			// Calculate u and v from current frame and uv_width and uv_height
			frame_.u = current_x_frame_ * frame_.uv_width - frame_.uv_width / 2;
			frame_.v = current_y_frame_ * frame_.uv_height - frame_.uv_height / 2;
		//}

		current_frame_++;
	}

	void SpriteAnimation::OnUpdateActivation()
	{
		ResetAnimation();
	}

	void SpriteAnimation::OnUpdateDeactivation()
	{
		finished_ = true;
	}

	void SpriteAnimation::CalculateUVDimensions()
	{
		frame_.uv_width = texture_width_ / x_frame_count_;
		frame_.uv_height = texture_height_ / y_frame_count_;
	}
}