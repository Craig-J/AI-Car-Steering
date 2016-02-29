#ifndef _SFX_CLOCK_H_
#define _SFX_CLOCK_H_
#include <SFML\System\Clock.hpp>

namespace sfx
{
	class Clock
	{
	public:

		Clock();

		void Start();
		void Stop();
		void Restart();
		sf::Time GetTimeElapsed();


	private:
		sf::Clock clock;
		sf::Time total_time;
		bool active_;
	};
}

#endif