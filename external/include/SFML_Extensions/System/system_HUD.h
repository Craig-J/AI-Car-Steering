#ifndef _SFX_SYSTEM_HUD_H_
#define _SFX_SYSTEM_HUD_H_
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include <SFML/Graphics.hpp>
#include "frame_clock.h"

namespace sfx
{
	class SystemHUD : public sf::Drawable
	{
		struct Stat
		{
			sf::Color color;
			std::string string;
		};

	public:

		SystemHUD(const sfx::FrameClock& _clock, const sf::Font& _font) :
			clock_(&_clock),
			font_(&_font)
		{}

	private:

		void draw(sf::RenderTarget& _render_target, sf::RenderStates _states) const
		{
			// Gather the available frame time statistics.
			const std::vector<Stat> stats = build();

			sf::Text text;
			text.setFont(*font_);
			text.setCharacterSize(20);
			text.setPosition(5.0f, 5.0f);

			// Draw the available frame time statistics.
			for (auto stat : stats)
			{
				text.setString(stat.string);
				text.setColor(stat.color);

				_render_target.draw(text, _states);

				// Next line.
				text.move(0.0f, 24.0f);
			}
		}

	private:

		template<typename T>
		static std::string format(std::string _name, std::string _resolution, T _value)
		{
			std::ostringstream os;
			os.precision(3);
			os << std::left << std::setw(5);
			os << _name << ": ";
			os << std::setw(5);
			os << _value << " " << _resolution;
			return os.str();
		}

		std::vector<Stat> build() const
		{
			const int count = 10;
			const Stat stats[count] = {
				{ sf::Color::Yellow, format("Time",  "(sec)", clock_->getTotalFrameTime().asSeconds()) },
				{ sf::Color::White,  format("Frame", "",      clock_->getTotalFrameCount()) },
				{ sf::Color::Green,  format("FPS",   "",      clock_->getFramesPerSecond()) },
				{ sf::Color::Green,  format("min.",  "",      clock_->getMinFramesPerSecond()) },
				{ sf::Color::Green,  format("avg.",  "",      clock_->getAverageFramesPerSecond()) },
				{ sf::Color::Green,  format("max.",  "",      clock_->getMaxFramesPerSecond()) },
				{ sf::Color::Cyan,   format("Delta", "(ms)",  clock_->getLastFrameTime().asMilliseconds()) },
				{ sf::Color::Cyan,   format("min.",  "(ms)",  clock_->getMinFrameTime().asMilliseconds()) },
				{ sf::Color::Cyan,   format("avg.",  "(ms)",  clock_->getAverageFrameTime().asMilliseconds()) },
				{ sf::Color::Cyan,   format("max.",  "(ms)",  clock_->getMaxFrameTime().asMilliseconds()) }
			};
			return std::vector<Stat>(&stats[0], &stats[0] + count);
		}

	private:

		const sf::Font* font_;
		const sfx::FrameClock* clock_;
	};
}

#endif // _SFX_SYSTEM_HUD_H_