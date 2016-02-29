#include "updateable_templated.h"

namespace agn
{
	template<typename UpdateMeasureType>
	bool Updateable_Templated<UpdateMeasureType>::IsTimeToUpdate()
	{
		// If the measure is lower or equal to the default constructed UpdateMeasureType
		// value then return indicating that we should update.
		if (elapsed_measure_ <= UpdateMeasureType()) return true;
		// Check if we have reached the target rate with the current measure.
		if (elapsed_measure_ >= target_rate_) return true;
		// Return indicating that no updating should be performed.
		return false;
	}

	template<typename UpdateMeasureType>
	void Updateable_Templated<UpdateMeasureType>::Update(const UpdateMeasureType _delta_measure)
	{
		// Only applied if this object is active for updating.
		if (!IsUpdateActive()) return;

		//	Increment for this update
		elapsed_measure_ += _delta_measure;
		if (IsTimeToUpdate())
		{
			// Measure since last performed update, not since last Update attempt.
			OnUpdate(elapsed_measure_);
			// Decrement elapsed measure.
			elapsed_measure_ -= target_rate_;
		}
	}

	template<typename UpdateMeasureType>
	void Updateable_Templated<UpdateMeasureType>::ActivateUpdate(bool _force_update = false)
	{
		if (active_) return;
		active_ = true;
		OnUpdateActivation();
		// Set measures to target rate to force an OnUpdate.
		if (_force_update)
		{
			elapsed_measure_ = target_rate_;
		}
	}

	template<typename UpdateMeasureType>
	void Updateable_Templated<UpdateMeasureType>::DeactivateUpdate()
	{
		if (!active_) return;
		active_ = false;
		OnUpdateDeactivation();
	}

	template<typename UpdateMeasureType>
	void Updateable_Templated<UpdateMeasureType>::ToggleUpdate()
	{
		if (active_)
			DeactivateUpdate();
		else ActivateUpdate();
	}

	template<typename UpdateMeasureType>
	void Updateable_Templated<UpdateMeasureType>::SetUpdateMeasureRate(const UpdateMeasureType _target_rate)
	{
		if (_target_rate >= UpdateMeasureType())
			target_rate_ = _target_rate;
	}

	template<typename UpdateMeasureType>
	void Updateable_Templated<UpdateMeasureType>::SetElapsedUpdateMeasure(const UpdateMeasureType _elapsed_measure)
	{
		if (_elapsed_measure >= UpdateMeasureType())
			elapsed_measure _ = _elapsed_measure;
	}
}