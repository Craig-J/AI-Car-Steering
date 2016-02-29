#include "input_manager.h"
namespace sfx
{

	void InputManager::SetKeyDown(sf::Keyboard::Key _key)
	{
		keyboard_.keys[_key] = true;
		if ((_key == sf::Keyboard::LAlt) || (_key == sf::Keyboard::RAlt))
		{
			keyboard_.alt = true;
		}
		else if ((_key == sf::Keyboard::LControl) || (_key == sf::Keyboard::RControl))
		{
			keyboard_.control = true;
		}
		else if ((_key == sf::Keyboard::LShift) || (_key == sf::Keyboard::RShift))
		{
			keyboard_.shift = true;
		}
		else if ((_key == sf::Keyboard::LSystem) || (_key == sf::Keyboard::RSystem))
		{
			keyboard_.system = true;
		}
	}

	void InputManager::SetKeyUp(sf::Keyboard::Key _key)
	{
		keyboard_.keys[_key] = false;
		if ((_key == sf::Keyboard::LAlt) || (_key == sf::Keyboard::RAlt))
		{
			keyboard_.alt = false;
		}
		else if ((_key == sf::Keyboard::LControl) || (_key == sf::Keyboard::RControl))
		{
			keyboard_.control = false;
		}
		else if ((_key == sf::Keyboard::LShift) || (_key == sf::Keyboard::RShift))
		{
			keyboard_.shift = false;
		}
		else if ((_key == sf::Keyboard::LSystem) || (_key == sf::Keyboard::RSystem))
		{
			keyboard_.system = false;
		}
	}

	void InputManager::SetMouse(sf::Vector2i _position)
	{
		if (mouse_.x != _position.x || mouse_.y != _position.y)
		{
			mouse_.x = _position.x;
			mouse_.y = _position.y;
			mouse_moved_ = true;
		}
	}

	void InputManager::SetButtonDown(sf::Mouse::Button _button)
	{
		mouse_.buttons[_button] = true;
	}

	void InputManager::SetButtonUp(sf::Mouse::Button _button)
	{
		mouse_.buttons[_button] = false;
	}

	void InputManager::Update()
	{
		mouse_moved_ = false; // Reset mouse moved for next frame otherwise it will always be true after first true case.
		previous_keyboard_ = keyboard_;
		previous_mouse_ = mouse_;
	}

	// Was this key pressed this update?
	const bool InputManager::KeyPressed(sf::Keyboard::Key _key) const
	{
		// If not pressed last frame but pressed this frame return true else false
		return ((!previous_keyboard_.keys[_key]) && (keyboard_.keys[_key]));
	}

	const bool InputManager::KeyDown(sf::Keyboard::Key _key)
	{
		return sf::Keyboard::isKeyPressed(_key);
	}

	// Was this key released this update?
	const bool InputManager::KeyReleased(sf::Keyboard::Key _key) const
	{
		// If pressed last frame but not pressed this frame return true else false
		return ((previous_keyboard_.keys[_key]) && (!keyboard_.keys[_key]));
	}

	// Was this mouse button pressed this update?
	const bool InputManager::ButtonPressed(sf::Mouse::Button _button) const
	{
		// If not pressed last frame but pressed this frame return true else false
		return ((!previous_mouse_.buttons[_button]) && (mouse_.buttons[_button]));
	}

	// Was this mouse button released this update?
	const bool InputManager::ButtonReleased(sf::Mouse::Button _button) const
	{
		// If pressed last frame but not pressed this frame return true else false
		return ((previous_mouse_.buttons[_button]) && (!mouse_.buttons[_button]));
	}
}