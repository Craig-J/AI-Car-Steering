#pragma once
#include <Fuzzylite\fl\Headers.h>
#include <SFML_Extensions\System\application.h>
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

	void GetUserConsoleInput();
	//bool ConvertAnswerToBool(std::string& _user_input);

	void UpdateGraphicsObjects();

	fl::Engine* engine_;

	fl::InputVariable* displacement_;
	fl::InputVariable* velocity_;

	fl::OutputVariable* steering_;

	fl::RuleBlock* rule_block_;

	struct Parameters
	{
		double displacement;
		double velocity;
		double steering;
	};
	Parameters initial;
	Parameters current;

	struct Set
	{
		double min, max;
		unsigned int mf_count;
	};
	Set displacement_set;
	Set velocity_set;
	Set steering_set;

	float timestep;
	unsigned int number_input_terms_;
	unsigned int number_output_terms_;

	bool real_time;
	bool paused;

	sf::Text timestep_;
	sf::Text output_;
	sfx::Sprite line_;
	sfx::Sprite car_;
	sfx::Sprite steering_indicator_;
	sfx::Sprite pause_overlay;
};