#include "AI_App.h"
#include <Agnostic\logger.h>
using agn::Log;
#include <Agnostic\string.h>
#include <SFML_Extensions\global.h>

namespace
{
	sf::Vector2f window_centre;
}

AI_App::AI_App(sf::RenderWindow& _window) :
	Application(_window),
	engine_(new fl::Engine("Fuzzy Car Controller")),
	rule_block_(new fl::RuleBlock),
	initial(),
	current(),
	paused(false)
{}

AI_App::~AI_App()
{}

bool AI_App::Initialize()
{
	if (font_.loadFromFile("NovaMono.ttf"))
	{
		Log::Message("NovaMono font loaded successfully.");
	}
	else
	{
		Log::Error("NovaMono font failed to load.");
	}

	window_centre = sf::Vector2f(window_.getSize().x * 0.5f, window_.getSize().y * 0.5f);

	output_.setFont(font_);
	output_.setCharacterSize(24);
	output_.setColor(sf::Color::White);
	output_.setPosition(window_centre.x, window_.getSize().y);

	line_ = sfx::Sprite(window_centre, Global::TextureManager.Load("arrow_white.png"));

	car_ = sfx::Sprite(window_centre, Global::TextureManager.Load("car.png"));

	steering_indicator_ = sfx::Sprite(window_centre, Global::TextureManager.Load("steering_indicator.png"));

	pause_overlay = sfx::Sprite(window_centre, Global::TextureManager.Load("pause_overlay.png"));

	displacement_ = new fl::InputVariable("Displacement", -1.0f, 1.0f);
	displacement_->addTerm(new fl::Ramp("FarLeft", -0.500, -1.000));
	displacement_->addTerm(new fl::Triangle("Left", -1.000, -0.500, 0.000));
	displacement_->addTerm(new fl::Triangle("Centre", -0.200, 0.000, 0.200));
	displacement_->addTerm(new fl::Triangle("Right", 0.000, 0.500, 1.000));
	displacement_->addTerm(new fl::Ramp("FarRight", 0.500, 1.000));
	engine_->addInputVariable(displacement_);

	velocity_ = new fl::InputVariable("Velocity", -1.0f, 1.0f);
	velocity_->addTerm(new fl::Ramp("FarLeft", -0.500,-1.000));
	velocity_->addTerm(new fl::Triangle("Left", -1.000, -0.500, 0.000));
	velocity_->addTerm(new fl::Triangle("Zero", -0.200, 0.000, 0.200));
	velocity_->addTerm(new fl::Triangle("Right", 0.000, 0.500, 1.000));
	velocity_->addTerm(new fl::Ramp("FarRight", 0.500, 1.000));
	engine_->addInputVariable(velocity_);

	steering_ = new fl::OutputVariable("Steering", -1.0f, 1.0f);
	steering_->addTerm(new fl::Ramp("FarLeft", -0.500, -1.000));
	steering_->addTerm(new fl::Triangle("Left", -1.000, -0.500, 0.000));
	steering_->addTerm(new fl::Triangle("Zero", -0.200, 0.000, 0.200));
	steering_->addTerm(new fl::Triangle("Right", 0.000, 0.500, 1.000));
	steering_->addTerm(new fl::Ramp("FarRight", 0.500, 1.000));
	engine_->addOutputVariable(steering_);

	// Far Left
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarRight then Steering is Zero", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Right then Steering is Right", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Zero then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Left then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarLeft then Steering is FarRight", engine_));

	// Left
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is FarRight then Steering is Left", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is Right then Steering is Zero", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is Zero then Steering is Right", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is Left then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is FarLeft then Steering is FarRight", engine_));

	// Centre
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarRight then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Right then Steering is Left", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Zero then Steering is Zero", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Left then Steering is Right", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarLeft then Steering is FarRight", engine_));

	// Right
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is FarRight then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is Right then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is Zero then Steering is Left", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is Left then Steering is Zero", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is FarLeft then Steering is Right", engine_));

	// Far Right
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarRight then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Right then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Zero then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Left then Steering is Left", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarLeft then Steering is Zero", engine_));

	engine_->addRuleBlock(rule_block_);

	engine_->configure("Minimum", "", "Minimum", "Maximum", "Centroid");

	std::string status;
	if (!engine_->isReady(&status))
	{
		throw fl::Exception("Engine not ready.\n"
							"The following errors were encountered:\n" + status, FL_AT);
	}

	Log::Message("Setup Successful.");
	std::cout << std::endl;
	Log::Message("Welcome to the Fuzzy Car Controller!");

	GetUserConsoleInput();

	return true;
}

void AI_App::CleanUp()
{
}

void AI_App::ProcessEvent(sf::Event& _event)
{
}

void AI_App::GetUserConsoleInput()
{
	Log::Message("Choose Displacement between -1 and 1 : ");
	std::cin >> initial.displacement;
	while (initial.displacement < -1 || initial.displacement > 1)
	{
		Log::Error("Displacement is invalid. Enter a displacement between -1 and 1: ");
		std::cin >> initial.displacement;
	}
	displacement_->setInputValue(initial.displacement);

	Log::Message("Choose a velocity between -1 and 1 : ");
	std::cin >> initial.velocity;
	while (initial.velocity < -1 || initial.velocity > 1)
	{
		Log::Error("Velocity is invalid. Enter a velocity between -1 and 1: ");
		std::cin >> initial.velocity;
	}
	velocity_->setInputValue(initial.velocity);

	engine_->process();
	initial.steering = steering_->getOutputValue();

	Log::Message("Engine initilization completed. Ready to run.");
	current = initial;

	UpdateGraphicsObjects();

	std::string answer;
	Log::Message("Animate car continuous or discretely? (C/D)");
	Log::Message("Continuous - car will animate in real time.");
	Log::Message("Discrete - car will animate one step at a time. (0.1s steps)");
	std::cin >> answer;
	while (!(answer == "C" || answer == "c" || answer == "D" || answer == "d"))
	{
		Log::Error("Invalid input. Please enter C for Continuous or D for Discrete.");
		std::cin >> answer;
	}
	if (answer == "C" || answer == "c")
	{
		real_time = true;
		paused = true;
		Log::Message("The SFML window will now animate the car.");
		Log::Message("Press spacebar to pause/unpause the animation.");
		Log::Message("Press R to reset the animation.");
		Log::Message("Press S to restart with new input values.");
	}
	else if (answer == "D" || answer == "d")
	{
		real_time = false;
		paused = false;
		Log::Message("The SFML window will now animate the car.");
		Log::Message("Press spacebar to step forward the animation.");
		Log::Message("Press R to reset the animation.");
		Log::Message("Press S to restart with new input values.");
	}
}

bool AI_App::ConvertAnswerToBool(std::string & _user_input)
{
	bool answer;
	if (_user_input == "Yes" || _user_input == "yes" || _user_input == "YES" || _user_input == "y")
	{
		answer = true;
	}
	else if (_user_input == "No" || _user_input == "no" || _user_input == "NO" || _user_input == "n")
	{
		answer = false;
	}
	else
	{
		Log::Error("Invalid Input. Please enter input again.");
		std::string new_answer;
		std::cin >> new_answer;
		return ConvertAnswerToBool(new_answer);
	}
	return answer;
}

void AI_App::UpdateGraphicsObjects()
{
	car_.setPosition(window_centre.x + current.displacement * 300.0f, car_.getPosition().y);
	car_.setRotation(current.velocity * 90.0f);

	steering_indicator_.setPosition(car_.getPosition());
	steering_indicator_.setRotation(current.steering * 90.0f);

	std::string output;
	output.append("Displacement: " + agn::to_string_precise(current.displacement, 1));
	output.append(" Velocity: " + agn::to_string_precise(current.velocity, 1));
	output.append(" Steering: " + agn::to_string_precise(current.steering, 1));
	output_.setString(output);
}

bool AI_App::Update()
{
	auto delta_time = clock_.getLastFrameTime();

	if(real_time)
	{
		if (!paused)
		{
			current.velocity += current.steering * delta_time.asSeconds();
			velocity_->setInputValue(current.velocity);

			current.displacement += current.velocity * delta_time.asSeconds();
			displacement_->setInputValue(current.displacement);

			engine_->process();
			current.steering = steering_->getOutputValue();

			UpdateGraphicsObjects();
		}

		if (Global::Input.KeyPressed(sf::Keyboard::Space))
		{
			paused = !paused;
		}
	}
	else
	{
		if (Global::Input.KeyPressed(sf::Keyboard::Space))
		{
			current.velocity += current.steering * 0.1f;
			velocity_->setInputValue(current.velocity);

			current.displacement += current.velocity * 0.1f;
			displacement_->setInputValue(current.displacement);

			engine_->process();
			current.steering = steering_->getOutputValue();

			UpdateGraphicsObjects();
		}
	}

	if (Global::Input.KeyPressed(sf::Keyboard::R))
	{
		current = initial;
	}

	if (Global::Input.KeyPressed(sf::Keyboard::S))
	{
		GetUserConsoleInput();
	}
	
	return true;
}

void AI_App::Render()
{
	window_.draw(line_);
	window_.draw(car_);
	window_.draw(steering_indicator_);

	output_.setOrigin(output_.getLocalBounds().width / 2.0f, output_.getLocalBounds().height);
	output_.setPosition(window_centre.x, window_.getSize().y - (output_.getLocalBounds().height));
	window_.draw(output_);

	if(paused)
		window_.draw(pause_overlay);
}

