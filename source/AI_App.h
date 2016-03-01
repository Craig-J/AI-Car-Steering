#pragma once
#include <SFML_Extensions\System\application.h>
#include <Fuzzylite\fl\Headers.h>
#include <memory>
#include <SFML_Extensions\Graphics\sprite.h>

class AI_App : public sfx::Application
{
public:
	AI_App(sf::RenderWindow&);
	~AI_App();

private:

	bool Initialize();
	void CleanUp();
	bool Update();
	void Render();
	void ProcessEvent(sf::Event& _event);

	bool ConvertAnswerToBool(std::string& _user_input);

	std::unique_ptr<fl::Engine> engine_;

	std::unique_ptr<fl::InputVariable> displacement_;
	std::unique_ptr<fl::InputVariable> velocity_;

	std::unique_ptr<fl::OutputVariable> steering_;

	std::unique_ptr<fl::RuleBlock> rule_block_;

	double displacement;
	double velocity;
	double steering;

	bool real_time;

	sf::Text output_;
	sfx::Sprite line_;
	sfx::Sprite car_;
};