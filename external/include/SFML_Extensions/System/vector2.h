#pragma once

#include <string>
#include <SFML/System/Vector2.hpp>

namespace sfx
{
	template<typename T>
	static float Dot(const sf::Vector2<T>& _first, const sf::Vector2<T>& _second)
	{
		return(_first.x * _second.x + _first->y * _second.y);
	}

	template<typename T>
	static sf::Vector2<T> Normalize(const sf::Vector2<T>& _vec)
	{
		return(_vec / Magnitude(_vec));
	}

	template<typename T>
	static sf::Vector2<T> Truncate(const sf::Vector2<T>& _vec, float _max)
	{
		if (_vec.Magnitude() > _max)
			return sf::Vector2<T>(Normalize(_vec) * _max);
		else
			return _vec;
	}

	template<typename T>
	static float Angle(const sf::Vector2<T>& _vec)
	{
		return std::atan2(_vec.x, _vec.y);
	}

	template<typename T>
	static float Magnitude(const sf::Vector2<T>& _vec)
	{
		return std::sqrt(std::pow(_vec.x, 2) + std::pow(_vec.y, 2));
	}

	template<typename T>
	static std::string to_str(const sf::Vector2<T>& _vec)
	{
		std::string string;
		string.append("[");
		string.append(std::to_string(_vec.x));
		string.append(", ");
		string.append(std::to_string(_vec.y));
		string.append("]");
		return string;
	}
}