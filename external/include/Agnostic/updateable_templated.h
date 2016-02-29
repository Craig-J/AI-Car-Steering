#ifndef _AGNOSTIC_UPDATEABLE_TEMPLATED_H_
#define _AGNOSTIC_UPDATEABLE_TEMPLATED_H_

namespace agn
{
	// Note this class is designed to be used with the basic data types.
	// It takes advantage of default initialization of the templated type.
	template<typename UpdateMeasureType>
	class Updateable_Templated
	{
	public:

		// Could be useful to track what the current measure type is, mostly for
		// programmer convenience.
		//enum class UpdateType { FRAMES, TIME } UpdateType;

		void Update(const UpdateMeasureType _delta_measure);

		//	Force implementation of update event
		virtual void OnUpdate(const UpdateMeasureType _delta_measure) = 0;

		//	Voluntary implementation of state-change events
		virtual void OnUpdateActivation() {}
		virtual void OnUpdateDeactivation() {}

		inline bool IsUpdateActive() const { return active_; }
		void ActivateUpdate(bool _force_update = false);
		void DeactivateUpdate();
		void ToggleUpdate();

		inline UpdateMeasureType UpdateMeasureRate() const { return target_rate_; }
		void SetUpdateMeasureRate(const UpdateMeasureType _target_rate);

		inline UpdateMeasureType ElapsedUpdateMeasure() const { return elapsed_measure_; }
		void SetElapsedUpdateMeasure(const UpdateMeasureType _elapsed_measure);

		// Default construction
		// No parameters, OnUpdate on every call to Update
		// Default constructs the UpdateMeasureType elapsed measure and target update rate
		// Target rate much be changed from 0 to begin limiting the update rate
		Updateable_Templated() :
			active_(true),
			elapsed_measure_(UpdateMeasureType()),
			target_rate_(UpdateMeasureType())
		{}

		virtual ~Updateable_Templated() {}
	private:

		UpdateMeasureType elapsed_measure_;
		UpdateMeasureType target_rate_;

		// May be useful to allow several measure types and converting between?
		// May be useful to allow different types for the measure and rate?
		/*template<typename _MeasureType, typename _RateType>
		struct UpdateTypeStructure
		{
			_MeasureType elapsed_measure;
			_RateType target_rate;
		};

		// Only use one measure type at a time
		union
		{
			UpdateTypeStructure<int, int> frames;
			UpdateTypeStructure<float, float> time;
		};*/

		bool active_;

		bool IsTimeToUpdate();

	};
}
#endif // _AGNOSTIC_UPDATEABLE_TEMPLATED_H_