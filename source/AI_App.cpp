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
	// AddTerm
	// Common function for adding any term type to a fl::variable.
	// To be honest this function isn't really needed.
	// Could have easily just used the constructors for each term type directly.
	template <class _Term_Type, typename ... _Arg_Types>
	void AddTerm(fl::Variable* _variable, std::string _term_name, _Arg_Types ... _args)
	{
		_variable->addTerm(new _Term_Type(_term_name, _args...));
	}

	// InitializeSet
	// Generates equally spaced and symmetrical-about-0 triangle terms in a range of -min to +max.
	// Triangle width/range is equal to double their spacing (1*spacing on either side of the triangle).
	// Edge terms are ramps (essentially half-triangles on the edge of the set).
	// Edge terms are named FarLeft and FarRight.
	// Centre term is named Centre.
	// Left terms are reverse numbered from the left edge to the centre in the manner FarLeft, Left(n), Left(n-1) ... Left1, centre.
	// Right terms are numbered from the centre to the right edge in the manner centre, Right1, ... Right(n-1), Right(n), FarRight.
	void InitializeSet(fl::Variable* _variable,
					   std::string _variable_name,
					   fl::scalar _min, fl::scalar _max,
					   unsigned int _number_of_terms)
	{
		_variable->setName(_variable_name);
		_variable->setRange(_min, _max);

		// Number of terms, minus the centre term, divided by 2 to get side, minus the edge term for the side.
		auto number_of_side_terms = ((_number_of_terms - 1) / 2) - 1;
		auto centre_index = number_of_side_terms + 1;
		fl::scalar term_spacing = _max / (number_of_side_terms + 1);

		// From testing, it seemed that terms with their half range equal to spacing worked best.
		auto term_half_range = term_spacing;

		for (unsigned int index = 0; index < _number_of_terms; ++index)
		{
			if (index == 0) // First term (far left), always index zero.
			{
				AddTerm<fl::Ramp>(_variable, "FarLeft", _min + term_half_range, _min);
			}
			else if (index == _number_of_terms - 1) // Last term (far right), always the last index.
			{
				AddTerm<fl::Ramp>(_variable, "FarRight", _max - term_half_range, _max);
			}
			else if (index == centre_index) // Centre term.
			{
				AddTerm<fl::Triangle>(_variable, "Centre", -term_half_range, 0.0, term_half_range);
			}
			else if (index < centre_index) // Terms indexed before the centre, all of the Left ones.
			{
				AddTerm<fl::Triangle>(_variable, "Left" + std::to_string(centre_index - index), // Reverse numbered from after the edge to the centre as n ... 1, centre.
									  _min + index * term_spacing - term_half_range,
									  _min + index * term_spacing,
									  _min + index * term_spacing + term_half_range);
			}
			else // Remaining terms, all of the Right ones.
			{
				AddTerm<fl::Triangle>(_variable, "Right" + std::to_string(index - centre_index), // Numbered from centre, 1 ... n.
									  _min + index * term_spacing - term_half_range,
									  _min + index * term_spacing,
									  _min + index * term_spacing + term_half_range);
			}
		}
	}

	/*std::string ConstructRule(unsigned int _displacement, unsigned int _velocity, unsigned int _steering,
							  int _d_direction = 1, int _v_direction = 1, int _s_direction = 1)
	{
		std::string output;
		output.append("if Displacement is ");
		switch (_d_direction)
		{
		case 0:
			output.append("FarLeft");
			break;
		case 1:
			output.append("Left" + std::to_string(_displacement));
			break;
		case 2:
			output.append("Centre");
			break;
		case 3:
			output.append("Right" + std::to_string(_displacement));
			break;
		case 4:
			output.append("FarRight");
			break;
		}
		output.append(" and Velocity is ");
		switch (_v_direction)
		{
		case 0:
			output.append("FarLeft");
			break;
		case 1:
			output.append("Left" + std::to_string(_velocity));
			break;
		case 2:
			output.append("Centre");
			break;
		case 3:
			output.append("Right" + std::to_string(_velocity));
			break;
		case 4:
			output.append("FarRight");
			break;
		}
		output.append(" then Steering is ");
		switch (_s_direction)
		{
		case 0:
			output.append("FarLeft");
			break;
		case 1:
			output.append("Left" + std::to_string(_steering));
			break;
		case 2:
			output.append("Centre");
			break;
		case 3:
			output.append("Right" + std::to_string(_steering));
			break;
		case 4:
			output.append("FarRight");
			break;
		}
		return output;
	}*/

	// InitializeRules
	// Successfully generates the rules on the trailing diagonal, the top left cell and the bottom right cell of the fuzzy associative map.
	// Will break if the input is not sensible, i.e. only odd numbers of terms work and mismatching the output and input term number will cause a crash.
	void InitializeRules(fl::Engine* _engine,
						 fl::RuleBlock* _rule_block,
						 unsigned int _number_input_terms,
						 unsigned int _number_output_terms)
	{
		auto max_diagonal = (_number_input_terms * 2) - 1;
		auto diagonal_centre_index = (max_diagonal - 1) / 2;

		// Don't know exactly what to call this variable.
		// It tracks the diagonal offset (in indices) away from the centre of the map that output starts being less extreme (not exact opposite steering)
		// Ended up being unused.
		auto magic_diagonal_offset = _number_input_terms - ((_number_output_terms - 1) / 2);

		// Iterating over the fuzzy associative map diagonally
		for (unsigned int diagonal_index = 0; diagonal_index < max_diagonal; ++diagonal_index)
		{
			unsigned int diagonal_term_count;
			if (diagonal_index <= diagonal_centre_index)
			{
				diagonal_term_count = diagonal_index + 1;
			}
			else
			{
				diagonal_term_count = max_diagonal - diagonal_index;
			}
			unsigned int this_diagonal_centre_index = (diagonal_term_count - 1) / 2;

			// Trailing diagonal - All the Steering = Centre rules
			if (diagonal_index == diagonal_centre_index)
			{
				for (int index = 0; index < diagonal_term_count; ++index)
				{
					if (index == 0)
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarRight then Steering is Centre", _engine));
					}
					else if (index == diagonal_term_count - 1)
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarLeft then Steering is Centre", _engine));
					}
					else if (index == this_diagonal_centre_index)
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Centre then Steering is Centre", _engine));
					}
					else if (index < this_diagonal_centre_index)
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(this_diagonal_centre_index - index)
															 + " and Velocity is Right" + std::to_string(this_diagonal_centre_index - index)
															 + " then Steering is Centre", _engine));
					}
					else
					{
						_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(index - this_diagonal_centre_index)
															 + " and Velocity is Left" + std::to_string(index - this_diagonal_centre_index)
															 + " then Steering is Centre", _engine));
					}
				}
			}
			else if (diagonal_index == 0) // Top left rule in the map
			{
				_rule_block->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarLeft then Steering is FarRight", _engine));
			}
			else if (diagonal_index == max_diagonal - 1) // Bottom right rule in the map
			{
				_rule_block->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarRight then Steering is FarLeft", _engine));
			}
			/*else  // If we're on the other diagonals
			{
				bool magic;
				if (diagonal_index < diagonal_centre_index)
				{
					if (diagonal_index >= magic_diagonal_offset)
						magic = true;
					else
						magic = false;
				}
				else
				{
					if (max_diagonal - diagonal_index >= magic_diagonal_offset)
						magic = true;
					else
						magic = false;
				}

				// For all rules in the diagonal
				for (int index = 0; index < diagonal_term_count; ++index)
				{
					if (index == 0)
					{
						if (magic)
						{
							_rule_block->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Right" + std::to_string(index - this_diagonal_centre_index)
																 + " then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
						}
						else
						{
							_rule_block->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Right" + std::to_string(index - this_diagonal_centre_index)
																 + " then Steering is FarRight", _engine));
						}
					}
					else if (index == diagonal_term_count - 1)
					{
						if (magic)
						{
							_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(this_diagonal_centre_index - index)
																 + " and Velocity is FarLeft then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
						}
						else
						{
							_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(this_diagonal_centre_index - index)
																 + " and Velocity is FarLeft then Steering is FarRight", _engine));
						}
					}
					if (index < this_diagonal_centre_index)
					{
						if (magic)
						{
							_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(this_diagonal_centre_index - index)
																 + " and Velocity is Right" + std::to_string(this_diagonal_centre_index - index)
																 + " then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
						}
						else
						{
							_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(this_diagonal_centre_index - index)
																 + " and Velocity is Right" + std::to_string(this_diagonal_centre_index - index)
																 + " then Steering is FarRight", _engine));
						}
					}
					else
					{
						if (magic)
						{
							_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(index - this_diagonal_centre_index)
																 + " and Velocity is Left" + std::to_string(index - this_diagonal_centre_index)
																 + " then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
						}
						else
						{
							_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(index - this_diagonal_centre_index)
																 + " and Velocity is Left" + std::to_string(index - this_diagonal_centre_index)
																 + " then Steering is FarRight", _engine));
						}
					}
				}
			}*/
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

	// Setup of term counts for NxNxM generalization.
	number_input_terms_ = 5;
	number_output_terms_ = 9;

	// Displacement Setup
	displacement_set.min = -1.0f;
	displacement_set.max = 1.0f;
	displacement_set.mf_count = number_input_terms_;

	InitializeSet(displacement_, "Displacement", 
				  displacement_set.min, displacement_set.max,
				  displacement_set.mf_count);
	engine_->addInputVariable(displacement_);

	// Velocity Setup
	velocity_set.min = -1.0f;
	velocity_set.max = 1.0f;
	velocity_set.mf_count = number_input_terms_;

	InitializeSet(velocity_, "Velocity",
				  velocity_set.min, velocity_set.max,
				  velocity_set.mf_count);
	engine_->addInputVariable(velocity_);

	// Steering Setup
	steering_set.min = -1.0f;
	steering_set.max = 1.0f;
	steering_set.mf_count = number_output_terms_;

	InitializeSet(steering_, "Steering",
				  steering_set.min, steering_set.max,
				  steering_set.mf_count);
	engine_->addOutputVariable(steering_);

	// Initialize the rules that we ended up generating succesfully.
	InitializeRules(engine_, rule_block_, number_input_terms_, number_output_terms_);

	// Manually write the rest. Auto-generated terms are left commented out.
	// Note that the names of variables are changed from the other FIS version to accommodate writing the variable FIS.

	// Far Left
	//rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarLeft then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Left1 then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Centre then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Right1 then Steering is Right1", engine_));
	//rule_block_->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is FarRight then Steering is Zero", engine_));

	// Left
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left1 and Velocity is FarLeft then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left1 and Velocity is Left1 then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left1 and Velocity is Centre then Steering is Right1", engine_));
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Left and Velocity is Right then Steering is Zero", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Left1 and Velocity is FarRight then Steering is Left1", engine_));

	// Centre
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarLeft then Steering is FarRight", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Left1 then Steering is Right1", engine_));
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Zero then Steering is Zero", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Right1 then Steering is Left1", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is FarRight then Steering is FarLeft", engine_));

	// Right
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right1 and Velocity is FarLeft then Steering is Right1", engine_));
	//rule_block_->addRule(fl::Rule::parse("if Displacement is Right and Velocity is Left then Steering is Zero", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right1 and Velocity is Centre then Steering is Left1", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right1 and Velocity is Right1 then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is Right1 and Velocity is FarRight then Steering is FarLeft", engine_));

	// Far Right
	//rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarLeft then Steering is Zero", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Left1 then Steering is Left1", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Centre then Steering is FarLeft", engine_));
	rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is Right1 then Steering is FarLeft", engine_));
	//rule_block_->addRule(fl::Rule::parse("if Displacement is FarRight and Velocity is FarRight then Steering is FarLeft", engine_));

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

/*bool AI_App::ConvertAnswerToBool(std::string & _user_input)
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
}*/

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



// First rule in the diagonal
/*if (index == 0)
{
// If this diagonal is past the "magic offset"
if (diagonal_index >= magic_diagonal_offset)
{
//
_rule_block->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Right" + std::to_string(index - this_diagonal_centre_index)
+ " then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
}
else
{
_rule_block->addRule(fl::Rule::parse("if Displacement is FarLeft and Velocity is Right"+ std::to_string(index - this_diagonal_centre_index)
+ " then Steering is FarRight", _engine));
}
}
// Last rule in the diagonal
else if (index == diagonal_term_count - 1)
{
if (diagonal_index >= magic_diagonal_offset)
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(this_diagonal_centre_index - index)
+ " and Velocity is FarLeft then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
}
else
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(this_diagonal_centre_index - index)
+ " and Velocity is FarLeft then Steering is FarRight", _engine));
}
}
// Middle rule in the diagonal (if one)
else if (index == this_diagonal_centre_index)
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Centre and Velocity is Centre then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
}
// Rules before the middle
else if (index < this_diagonal_centre_index)
{
if (diagonal_index >= magic_diagonal_offset)
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(this_diagonal_centre_index - index)
+ " and Velocity is Right" + std::to_string(index - this_diagonal_centre_index)
+ " then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
}
else
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(this_diagonal_centre_index - index)
+ " and Velocity is Right" + std::to_string(index - this_diagonal_centre_index)
+ " then Steering is FarRight", _engine));
}
}
// Rules after the middle
else
{
if (diagonal_index >= magic_diagonal_offset)
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(this_diagonal_centre_index - index)
+ " and Velocity is Right" + std::to_string(index - this_diagonal_centre_index)
+ " then Steering is Right" + std::to_string(diagonal_centre_index - diagonal_index), _engine));
}
else
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(this_diagonal_centre_index - index)
+ " and Velocity is Right" + std::to_string(index - this_diagonal_centre_index)
+ " then Steering is FarRight", _engine));
}
_rule_block->addRule(fl::Rule::parse("if Displacement is Left" + std::to_string(centre_index - index)
+ " and Velocity is Right" + std::to_string(index - centre_index)
+ " then Steering is FarRight", _engine));
}
}
}
else
{
for (int index = 0; index < diagonal_term_count; ++index)
{
if (max_diagonal - diagonal_index - 1 >= magic_diagonal_offset)
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(index - this_diagonal_centre_index)
+ " and Velocity is Left" + std::to_string(index - this_diagonal_centre_index)
+ " then Steering is Left" + std::to_string(diagonal_index - diagonal_centre_index), _engine));
}
else
{
_rule_block->addRule(fl::Rule::parse("if Displacement is Right" + std::to_string(index - this_diagonal_centre_index)
+ " and Velocity is Left" + std::to_string(index - this_diagonal_centre_index)
+ " then Steering is FarLeft", _engine));
}
}
}*/