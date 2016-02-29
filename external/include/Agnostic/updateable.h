#ifndef _AGNOSTIC_UPDATEABLE_H_
#define _AGNOSTIC_UPDATEABLE_H_

namespace agn
{
	class Updateable
	{
	public:

		void Update(const float delta_time);

		//	Force implementation of update event
		virtual void OnUpdate(const float delta_time) = 0;

		//	Voluntary implementation of state-change events
		virtual void OnUpdateActivation() {}
		virtual void OnUpdateDeactivation() {}

		inline bool IsUpdateActive() const { return active_; }
		void ActivateUpdate(bool _force_update = false);
		void DeactivateUpdate();
		void ToggleUpdate();

		inline int GetFrameRate() const { return frame_rate_; }
		void SetUpdateFrameRate(const int frame_rate);

		inline float GetFrameTime() const { return frame_time_; }
		void SetUpdateFrameTime(const float frame_time);

		// Default construction
		// No parameters, OnUpdate on every call to Update.
		// Update measure rates must be changed from 0 to limit Update rate.
		Updateable() :
			active_(true),
			frame_rate_(0),
			frame_count_(0),
			frame_time_(0.0f),
			time_elapsed_(0.0f)
		{}

		virtual ~Updateable() {}


	private:

		int frame_rate_;		// target frame rate.
		int frame_count_;		// frames since last update.
		float frame_time_;		// target time between updates.
		float time_elapsed_;	// time since last update.
		bool active_;

		bool IsTimeToUpdate();

	};
}
#endif // _AGNOSTIC_UPDATEABLE_H_