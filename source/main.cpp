#include <cstdlib> // For EXIT_SUCCESS
#include <agnostic\logger.h>
using agn::Log;
#include "AI_App.h"

int main()
{
	Log::EnableConsole();
	Log::Message("Successfully started!");
	sf::RenderWindow window(sf::VideoMode(800, 600), "AI Coursework - Craig Jeffrey (1203086)");
	AI_App application(window);
	application.Run();
	Log::Message("Successfully exited!");
	return EXIT_SUCCESS;
}