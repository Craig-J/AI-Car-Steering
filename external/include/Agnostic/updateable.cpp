#include "updateable.h"

namespace agn
{
	bool Updateable::IsTimeToUpdate()
	{
		// if both measures are 0 then return indicating that we should update.
		if (frame_rate_ <= 0 && frame_time_ <= 0.0f) return true;
		// Check if frame rate is to be used and if it has been reached.
		if (frame_rate_ > 0 && frame_count_ >= frame_rate_) return true;
		// check if frame time is to be used and if it has been reached.
		if (frame_time_ > 0.0f && time_elapsed_ >= frame_time_) return true;
		// return indicating that neither of the measures have been reached and no updating should be performed.
		return false;
	}

	void Updateable::Update(const float _delta_time)
	{
		// Only applied if this object is active for updating.
		if (!IsUpdateActive()) return;

		//	Increment for this frame
		++frame_count_;
		time_elapsed_ += _delta_time;
		if (IsTimeToUpdate())
		{
			// parameter is the time since OnUpdate was last called, not the time since this function was last called.
			OnUpdate(time_elapsed_);
			// Decrement measure counts.
			frame_count_ -= frame_rate_;
			time_elapsed_ -= frame_time_;
		}
	}

	void Updateable::ActivateUpdate(bool _force_update)
	{
		if (active_) return;
		active_ = true;
		OnUpdateActivation();
		// Set frame measures to force OnUpdate at the next frame.
		if (_force_update)
		{
			frame_count_ = frame_rate_;
			time_elapsed_ = frame_time_;
		}
	}

	void Updateable::DeactivateUpdate()
	{
		if (!active_) return;
		active_ = false;
		OnUpdateDeactivation();
	}

	void Updateable::ToggleUpdate()
	{
		if (active_)
			DeactivateUpdate();
		else ActivateUpdate();
	}

	void Updateable::SetUpdateFrameRate(const int _frame_rate)
	{
		if (_frame_rate >= 0)
			frame_rate_ = _frame_rate;
	}

	void Updateable::SetUpdateFrameTime(const float _frame_time)
	{
		if (_frame_time >= 0.0f)
			frame_time_ = _frame_time;
	}
}