#ifndef _SFX_INPUT_MANAGER_H_
#define _SFX_INPUT_MANAGER_H_

#include <SFML\Window.hpp>

namespace sfx
{
	class InputManager
	{
	public:

		struct Mouse
		{
			int x, y;
			bool buttons[sf::Mouse::ButtonCount];
		};

		struct Keyboard
		{
			bool keys[sf::Keyboard::KeyCount];
			bool alt;
			bool control;
			bool shift;
			bool system;
		};

		////// MUTATORS
		// To move the mouse in realtime use the sf::Mouse setPosition functions.

		// Use these with events to update current objects at start of frame

		void SetKeyDown(sf::Keyboard::Key);
		void SetKeyUp(sf::Keyboard::Key);
		void SetMouse(sf::Vector2i position);
		void SetButtonDown(sf::Mouse::Button);
		void SetButtonUp(sf::Mouse::Button);

		// Call before event processing at start of frame or
		// at end after all input logic is handled.
		// Resets mousemoved and updates previous frame objects.
		void Update();

		////// ACCESSORS

		// This frames mouse data.
		inline const Mouse GetMouse() const { return mouse_; }

		// This frames keyboard data.
		inline const Keyboard GetKeyboard() const { return keyboard_; }

		// Last frames mouse data.
		inline const Mouse GetPreviousMouse() const { return previous_mouse_; }

		// Last frames keyboard data.
		inline const Keyboard GetPreviousKeyboard() const { return previous_keyboard_; }

		// Was the mouse moved this update?
		inline bool MouseMoved() const { return mouse_moved_; }

		// Was this key pressed this update?
		const bool KeyPressed(sf::Keyboard::Key) const;

		// Is this key currently down?
		const bool static KeyDown(sf::Keyboard::Key);

		// Was this key released this update?
		const bool KeyReleased(sf::Keyboard::Key) const;

		// Was this mouse button pressed this update?
		const bool ButtonPressed(sf::Mouse::Button) const;

		// Was this mouse button released this update?
		const bool ButtonReleased(sf::Mouse::Button) const;

	private:
		
		bool mouse_moved_;

		// TODO(Craig): Changes these to a container and add a history depth variable later.

		Mouse mouse_;
		Mouse previous_mouse_;
		Keyboard keyboard_;
		Keyboard previous_keyboard_;

	};
}

#endif // _SFX_INPUT_MANAGER_H_