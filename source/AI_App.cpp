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
	initial(),
	current(),
	paused(false),
	timestep(0.1f)
{}

AI_App::~AI_App()
{}

namespace
{
	template <class _Term_Type, typename ... _Arg_Types>
	void AddTerm(fl::Variable* _variable, std::string _term_name, _Arg_Types ... _args)
	{
		_variable->addTerm(new _Term_Type(_term_name, _args...));
	}

	void InitializeSet(fl::Variable* _variable,
					   std::string _variable_name,
					   fl::scalar _min, fl::scalar _max,
					   unsigned int _number_of_terms,
					   fl::scalar _term_range)
	{
		_variable->setName(_variable_name);
		_variable->setRange(_min, _max);

		bool even_set = (_number_of_terms % 2 == 0);

		// Number of terms, minus the centre term (if odd), divided by 2 for one side, minus the edge term for a side.
		auto number_of_side_terms = ((_number_of_terms - even_set) / 2) - 1;
		auto centre_index = number_of_side_terms + 1;
		fl::scalar term_spacing = _max / (number_of_side_terms + 1);

		for (unsigned int index = 0; index < _number_of_terms; ++index)
		{
			if (index == 0)
			{
				AddTerm<fl::Ramp>(_variable, "FarLeft", _min + _term_range, _min);
			}
			else if (index == _number_of_terms - 1)
			{
				AddTerm<fl::Ramp>(_variable, "FarRight", _max - _term_range, _max);
			}
			else if (index == centre_index && even_set == false)
			{
				AddTerm<fl::Triangle>(_variable, "Centre", -_term_range, 0.0, _term_range);
			}
			else if(index < centre_index)
			{
				AddTerm<fl::Triangle>(_variable, "Left",// + std::to_string(left_index),
									  _min + index * term_spacing - _term_range,
									  _min + index * term_spacing,
									  _min + index * term_spacing + _term_range);
			}
			else
			{
				AddTerm<fl::Triangle>(_variable, "Right",// + std::to_string(right_index),
									  _min + index * term_spacing - _term_range,
									  _min + index * term_spacing,
									  _min + index * term_spacing + _term_range);
			}
		}
	}

	void InitializeRules(fl::Engine* _engine,
						 fl::RuleBlock* _rule_block,
						 unsigned int _number_input_terms,
						 unsigned int _number_output_terms)
	{
		auto number_of_rules = std::pow(_number_input_terms, 2);
		auto centre_index = ((_number_input_terms - 1) / 2);

		for (unsigned int x_index = 0; x_index < _number_input_terms; ++x_index)
		{
			for (unsigned int y_index = 0; y_index < _number_input_terms; ++y_index)
			{
				if (x_index == -y_index)
				{
					if (x_index == 0)
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is FurthestLeft and Velocity is FurthestRight then Steering is Centre", _engine));
					}
					else if (x_index == _number_input_terms - 1)
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is FurthestRight and Velocity is FurthestLeft then Steering is Centre", _engine));
					}
					else if (x_index == centre_index)
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Centre then Steering is Centre", _engine));
					}
					else if (x_index < centre_index)
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(x_index) + " and Velocity is Centre then Steering is Centre", _engine));
					}
					else
					{

					}
				}
			}
		}
	}
}

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

	timestep_.setFont(font_);
	timestep_.setCharacterSize(24);
	timestep_.setColor(sf::Color(255, 69, 0));
	timestep_.setPosition(window_centre.x, 0.0f);

	line_ = sfx::Sprite(window_centre, Global::TextureManager.Load("arrow_white.png"));

	car_ = sfx::Sprite(window_centre, Global::TextureManager.Load("car.png"));

	steering_indicator_ = sfx::Sprite(window_centre, Global::TextureManager.Load("steering_indicator.png"));

	pause_overlay = sfx::Sprite(window_centre, Global::TextureManager.Load("pause_overlay.png"));

	InitializeSet(displacement_, "Displacement", -1.0f, 1.0f, 3, 1.0f);
	engine_->addInputVariable(displacement_);

	InitializeSet(velocity_, "Velocity", -1.0f, 1.0f, 3, 1.0f);
	engine_->addInputVariable(velocity_);

	InitializeSet(steering_, "Steering", -1.0f, 1.0f, 5, 0.5f);
	engine_->addOutputVariable(steering_);

	// Far Left
	/*rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarRight then Steering is Centre", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Right then Steering is Right", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Centre then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Left then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarLeft then Steering is FarRight", engine_));*/

	// Left
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is FarRight then Steering is Left", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarRight then Steering is Centre", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Centre then Steering is Right", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarLeft then Steering is FarRight", engine_));
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is FarLeft then Steering is FarRight", engine_));

	// Centre
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarRight then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarRight then Steering is Left", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Centre then Steering is Centre", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarLeft then Steering is Right", engine_));
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarLeft then Steering is FarRight", engine_));

	// Right
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is FarRight then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarRight then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Centre then Steering is Left", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarLeft then Steering is Centre", engine_));
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is FarLeft then Steering is Right", engine_));

	// Far Right
	/*rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarRight then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Right then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Centre then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Left then Steering is Left", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarLeft then Steering is Centre", engine_));*/

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
	Log::Message("Discrete - car will animate one step at a time.");
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
		timestep = 0.1f;
		Log::Message("The SFML window will now animate the car.");
		Log::Message("Press spacebar to step forward the animation.");
		Log::Message("Default timestep is 0.1.");
		Log::Message("Press T to increase timestep by 0.1.");
		Log::Message("Press F to decrease timestep by 0.1.");
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
	output.append("Displacement: " + agn::to_string_precise(current.displacement, 3));
	output.append(" Velocity: " + agn::to_string_precise(current.velocity, 3));
	output.append(" Steering: " + agn::to_string_precise(current.steering, 3));
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
			current.velocity += current.steering * timestep;
			velocity_->setInputValue(current.velocity);

			current.displacement += current.velocity * timestep;
			displacement_->setInputValue(current.displacement);

			engine_->process();
			current.steering = steering_->getOutputValue();

			UpdateGraphicsObjects();
		}
	}

	if (Global::Input.KeyPressed(sf::Keyboard::R))
	{
		current = initial;
		UpdateGraphicsObjects();
	}

	if (Global::Input.KeyPressed(sf::Keyboard::S))
	{
		GetUserConsoleInput();
	}

	if (Global::Input.KeyPressed(sf::Keyboard::T))
	{
		timestep += 0.1;
	}
	if (Global::Input.KeyPressed(sf::Keyboard::F))
	{
		timestep -= 0.1;
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

	if (!real_time)
	{
		timestep_.setString("Timestep: " + agn::to_string_precise(timestep, 1));
		timestep_.setOrigin(timestep_.getLocalBounds().width / 2.0f, timestep_.getLocalBounds().height);
		timestep_.setPosition(window_centre.x, (timestep_.getLocalBounds().height));
		window_.draw(timestep_);
	}

	if(paused)
		window_.draw(pause_overlay);
}

