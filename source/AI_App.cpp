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
	displacement_(new fl::InputVariable),
	velocity_(new fl::InputVariable),
	steering_(new fl::OutputVariable),
	rule_block_(new fl::RuleBlock),
	displacement(double()),
	velocity(double()),
	steering(double())
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

	displacement_->setEnabled(true);
	displacement_->setName("Displacement");
	displacement_->setRange(-1.000, 1.000);
	displacement_->addTerm(new fl::Triangle("FarLeft", -1.500, -1.000, -0.500));
	displacement_->addTerm(new fl::Triangle("Left", -1.000, -0.500, 0.000));
	displacement_->addTerm(new fl::Triangle("Centre", -0.200, 0.000, 0.200));
	displacement_->addTerm(new fl::Triangle("Right", 0.000, 0.500, 1.000));
	displacement_->addTerm(new fl::Triangle("FarRight", 0.500, 1.000, 1.500));
	engine_->addInputVariable(displacement_.get());

	velocity_->setEnabled(true);
	velocity_->setName("Velocity");
	velocity_->setRange(-1.000, 1.000);
	velocity_->addTerm(new fl::Triangle("FarLeft", -1.500, -1.000, -0.500));
	velocity_->addTerm(new fl::Triangle("Left", -1.000, -0.500, 0.000));
	velocity_->addTerm(new fl::Triangle("Zero", -0.200, 0.000, 0.200));
	velocity_->addTerm(new fl::Triangle("Right", 0.000, 0.500, 1.000));
	velocity_->addTerm(new fl::Triangle("FarRight", 0.500, 1.000, 1.500));
	engine_->addInputVariable(velocity_.get());


	steering_->setName("Steering");
	steering_->setRange(-1.000, 1.000);
	steering_->addTerm(new fl::Triangle("FarLeft", -1.500, -1.000, -0.500));
	steering_->addTerm(new fl::Triangle("Left", -1.000, -0.500, 0.000));
	steering_->addTerm(new fl::Triangle("Zero", -0.200, 0.000, 0.200));
	steering_->addTerm(new fl::Triangle("Right", 0.000, 0.500, 1.000));
	steering_->addTerm(new fl::Triangle("FarRight", 0.500, 1.000, 1.500));
	engine_->addOutputVariable(steering_.get());

	// Far Left
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarRight then Steering is Zero", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Right then Steering is Right", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Zero then Steering is FarRight", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Left then Steering is FarRight", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarLeft then Steering is FarRight", engine_.get()));

	// Left
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is FarRight then Steering is Left", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is Right then Steering is Zero", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is Zero then Steering is Right", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is Left then Steering is FarRight", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is FarLeft then Steering is FarRight", engine_.get()));

	// Centre
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarRight then Steering is FarLeft", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Right then Steering is Left", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Zero then Steering is Zero", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Left then Steering is Right", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarLeft then Steering is FarRight", engine_.get()));

	// Right
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is FarRight then Steering is FarLeft", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is Right then Steering is FarLeft", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is Zero then Steering is Left", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is Left then Steering is Zero", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is FarLeft then Steering is Right", engine_.get()));

	// Far Right
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarRight then Steering is FarLeft", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Right then Steering is FarLeft", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Zero then Steering is FarLeft", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Left then Steering is Left", engine_.get()));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarLeft then Steering is Zero", engine_.get()));

	engine_->addRuleBlock(rule_block_.get());

	engine_->configure("Minimum", "", "Minimum", "Maximum", "Centroid");

	std::string status;
	if (!engine_->isReady(&status))
	{
		throw fl::Exception("Engine not ready.\n"
							"The following errors were encountered:\n" + status, FL_AT);
	}

	Log::Message("Fuzzy Car Controller Initialized.");

	Log::Message("Choose Displacement between -1 and 1 : ");
	std::cin >> displacement;
	while (displacement < -1 || displacement > 1)
	{
		Log::Error("Displacement is invalid. Enter a displacement between -1 and 1: ");
		std::cin >> displacement;
	}
	displacement_->setInputValue(displacement);

	Log::Message("Choose a velocity between -1 and 1 : ");
	std::cin >> velocity;
	while (velocity < -1 || velocity > 1)
	{
		Log::Error("Velocity is invalid. Enter a velocity between -1 and 1: ");
		std::cin >> velocity;
	}
	velocity_->setInputValue(velocity);

	std::string answer;
	Log::Message("Animate car continuous or discretely? (C/D)");
	Log::Message("Continuous - car will animate in real time.");
	Log::Message("Discrete - car steps once every time spacebar is pressed.");
	std::cin >> answer;
	while (!(answer == "C" || answer == "c" || answer == "D" || answer == "d"))
	{
		Log::Error("Invalid input. Please enter C for Continuous or D for Discrete.");
		std::cin >> answer;
	}
	if (answer == "C" || answer == "c")
	{
		real_time = true;
	}
	else if (answer == "D" || answer == "d")
	{
		real_time = false;
	}

	return true;
}

void AI_App::CleanUp()
{
}

void AI_App::ProcessEvent(sf::Event& _event)
{
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

bool AI_App::Update()
{
	auto delta_time = clock_.getLastFrameTime();

	if(real_time)
	{
		velocity += steering * delta_time.asSeconds();
		velocity_->setInputValue(velocity);

		displacement += velocity * delta_time.asSeconds();
		displacement_->setInputValue(displacement);

		car_.setPosition(window_centre.x + displacement * 300.0f, car_.getPosition().y);
		car_.setRotation(steering * 90.0f);

		engine_->process();
		steering = steering_->getOutputValue();

		std::string output;
		output.append("Displacement: "+ agn::to_string_precise(displacement, 1));
		output.append(" Velocity: " + agn::to_string_precise(velocity, 1));
		output.append(" Steering: " + agn::to_string_precise(steering, 1));
		output_.setString(output);
	}
	else
	{
		if (Global::Input.KeyPressed(sf::Keyboard::Space))
		{
			velocity += steering * 0.1f;
			velocity_->setInputValue(velocity);

			displacement += velocity * 0.1f;
			displacement_->setInputValue(displacement);

			car_.setPosition(window_centre.x + displacement * 300.0f, car_.getPosition().y);
			car_.setRotation(steering * 90.0f);

			engine_->process();
			steering = steering_->getOutputValue();

			std::string output;
			output.append("Displacement: " + agn::to_string_precise(displacement, 1));
			output.append(" Velocity: " + agn::to_string_precise(velocity, 1));
			output.append(" Steering: " + agn::to_string_precise(steering, 1));
			output_.setString(output);
		}
	}
	
	return true;
}

void AI_App::Render()
{
	window_.draw(line_);
	window_.draw(car_);

	output_.move(0.0f, -output_.getLocalBounds().height);
	window_.draw(output_);
	output_.move(0.0f, output_.getLocalBounds().height);
}

