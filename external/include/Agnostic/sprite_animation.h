#ifndef _AGNOSTIC_SPRITE_ANIMATION_H_
#define _AGNOSTIC_SPRITE_ANIMATION_H_
#include "updateable.h"

namespace agn
{
	class SpriteAnimation : public agn::Updateable
	{
	public:

		// Frame
		// Data structure describing uv properties of one frame of animation
		struct Frame
		{
			int u, v;
		};

		// DynamicFrame
		// Frame with additional data for animations where the uv dimensions change
		struct DynamicFrame : public Frame
		{
			int uv_width, uv_height;
		};

		// Enum describing how to traverse the spritesheet
		// Note:	NONLINEAR, SCROLLXY && SCROLL_YX have no implementations currently
		enum class SPRITESHEET_TYPE { /*NONLINEAR,*/ SCROLL_X, SCROLL_Y, /*SCROLL_XY, SCROLL_YX*/ };

		// Default constructor: Sets up a SCROLL_X spritesheet with no update constraints (updates every time update is called)
		SpriteAnimation(const int _texture_width,
			const int _texture_height,
			const float _frame_duration = 0.0f,
			const int _frame_count = 1,
			const int _current_frame = 1,
			const bool _looping = false);

		virtual SpriteAnimation::~SpriteAnimation() {}

		// VIRTUAL FUNCTIONS
		// Optional

			// OnAnimationLoop
			// Called every time an animation loops
			virtual void OnAnimationLoop(){};

			// OnAnimationStart
			// Called when the animation begins
			virtual void OnAnimationStart(){};

			// OnAnimationEnd
			// Called when the animation finishes
			virtual void OnAnimationEnd(){};

		// ACCESSORS

			// GetFrame
			// Out:		Struct of current frame (u, v)
			// Note:	Call this after updating the animation to get the result
			inline Frame GetFrame() const { return frame_; }

			// GetDynamicFrame
			// Out:		Struct of current frame (u, v, uv_width, uv_height)
			// Note:	Call this after updating the animation to get the result
			inline DynamicFrame GetDynamicFrame() const { return frame_; }

			// CurrentFrame
			// In:		Frame to check for
			// Out:		True if the current frame is the frame to check for
			inline bool CurrentFrame(int _frame) const { return(_frame == current_frame_); }

			// IsFinished
			// Out:		True if the animation has finished
			inline bool IsFinished() const { return finished_; }

		// MUTATORS

			// ResetAnimation
			// Resets animation to it's initial state
			// Note:	This function does not affect the animations current update state
			void ResetAnimation();

			// SetSpriteSheetType
			// IN: Enumeration of type to set
			void SetSpriteSheetType(const SPRITESHEET_TYPE type);
		
			// SetFrameCount
			// IN:		Total number of frames
			//			(optional) Number of columns (x frames)
			//			(optional) Number of rows (y frames)
			// Notes:	X and Y counts are ignored for scroll_x and scroll_y sheet types
			//			Default frame count in unused dimensions should be 1 - default argument is 1
			void SetFrameCount(const int total_frame_count, const int x_count = 1, const int y_count = 1);

			// SetLooping
			// IN:		Whether to loop or not
			//			(optional) Number of times to loop (default 0 = infinite)
			void SetLooping(const bool looping, const int number_of_loops = 0);

			// SetOffset
			// IN:		Start frame number and end frame number
			// Note:	This function only alters animation playback, not animation frame counts
			//			End frame should not be higher than animation total frame count
			void SetOffset(const int start_frame, const int end_frame);

	protected:

		// State variables
		bool looping_;
		bool finished_;

		// Initial/nondynamic variables
		int texture_width_;
		int texture_height_;
		SPRITESHEET_TYPE sheet_type_;
		int start_frame_;
		int end_frame_;
		int animation_frame_count_;
		int x_frame_count_;
		int y_frame_count_;
		int loop_count_;

		// Dynamic variables
		DynamicFrame frame_;
		int current_frame_;
		int current_x_frame_;
		int current_y_frame_;
		int current_loop_;

	private:

		// Updateable implementations
		void OnUpdate(const float delta_time);
		void OnUpdateActivation();
		void OnUpdateDeactivation();

		// Helper functions
		void CalculateUVDimensions();
	};
}
#endif // _AGNOSTIC_SPRITE_ANIMATION_H_