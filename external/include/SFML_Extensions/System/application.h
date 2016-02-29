#ifndef _SFX_APPLICATION_H_
#define _SFX_APPLICATION_H_

#include <SFML\Graphics\RenderWindow.hpp>
#include "frame_clock.h"
#include "system_HUD.h"

namespace sfx
{
	class Application
	{
	public:

		Application(sf::RenderWindow& = sf::RenderWindow(sf::VideoMode::getDesktopMode(), "SFML Application"));
		virtual ~Application() {}

		void Run();

	protected:

		sfx::FrameClock clock_;
		sf::RenderWindow& window_;
		sf::Font font_;

	private:

		virtual bool Initialize() = 0;
		virtual void CleanUp() = 0;
		virtual bool Update() = 0;
		virtual void Render() = 0;
		// Default event handling does nothing.
		virtual void ProcessEvent(sf::Event& _event) {}

		void EventLoop();

		SystemHUD hud_;
		bool running_;
	};
}
#endif // _SFX_APPLICATION_H_