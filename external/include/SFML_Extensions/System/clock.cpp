#include "clock.h"

namespace sfx
{

	Clock::Clock() :
		clock(),
		total_time(),
		active_(true)
	{}

	void Clock::Start()
	{
		if (!active_)
		{
			clock.restart();
			active_ = true;
		}
	}

	void Clock::Stop()
	{
		if (active_)
		{
			total_time += clock.restart();
			active_ = false;
		}
	}

	void Clock::Restart()
	{
		clock.restart();
		total_time = sf::Time::Zero;
	}

	sf::Time Clock::GetTimeElapsed()
	{
		if (active_)
		{
			total_time += clock.restart();
		}
		return total_time;
	}

}