#include "animation.h"

namespace sfx
{
	sf::IntRect Animation::GetFrame() const
	{
		sf::IntRect rect;
		DynamicFrame frame = GetDynamicFrame();
		rect.left = frame.u - frame.uv_width / 2;
		rect.width = frame.uv_width;
		rect.top = frame.v - frame.uv_height / 2;
		rect.height = frame.uv_height;

		return rect;
	}
}