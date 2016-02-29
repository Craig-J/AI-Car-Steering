#include <Fuzzylite\Headers.h>
#include <stdio.h>
#include <Windows.h>

int main(int argc, char* argv[])
{
	using namespace std;
	using namespace fl;
	Engine* engine = new Engine("Fuzzy Car Controller");

	InputVariable* Displacement = new InputVariable;
	Displacement->setEnabled(true);
	Displacement->setName("Displacement");
	Displacement->setRange(-1.000, 1.000);
	Displacement->addTerm(new Triangle("LargeLeft", -1.500, -1.000, -0.500));
	Displacement->addTerm(new Triangle("MediumLeft", -1.000, -0.500, 0.000));
	Displacement->addTerm(new Triangle("Centre", -0.200, 0.000, 0.200));
	Displacement->addTerm(new Triangle("MediumRight", 0.000, 0.500, 1.000));
	Displacement->addTerm(new Triangle("LargeRight", 0.500, 1.000, 1.500));
	engine->addInputVariable(Displacement);

	InputVariable* Velocity = new InputVariable;
	Velocity->setName("Velocity");
	Velocity->setEnabled(true);
	Velocity->setRange(-1.000, 1.000);
	Velocity->addTerm(new Triangle("LargeLeft", -1.500, -1.000, -0.500));
	Velocity->addTerm(new Triangle("MediumLeft", -1.000, -0.500, 0.000));
	Velocity->addTerm(new Triangle("Zero", -0.200, 0.000, 0.200));
	Velocity->addTerm(new Triangle("MediumRight", 0.000, 0.500, 1.000));
	Velocity->addTerm(new Triangle("LargeRight", 0.500, 1.000, 1.500));
	engine->addInputVariable(Velocity);


	OutputVariable* Steering = new OutputVariable;
	Steering->setName("Steering");
	Steering->setRange(-1.000, 1.000);
	Steering->addTerm(new Triangle("LargeLeft", -1.500, -1.000, -0.500));
	Steering->addTerm(new Triangle("MediumLeft", -1.000, -0.500, 0.000));
	Steering->addTerm(new Triangle("Zero", -0.200, 0.000, 0.200));
	Steering->addTerm(new Triangle("MediumRight", 0.000, 0.500, 1.000));
	Steering->addTerm(new Triangle("LargeRight", 0.500, 1.000, 1.500));
	engine->addOutputVariable(Steering);

	RuleBlock* ruleBlock = new RuleBlock;
	ruleBlock->setEnabled(true);
	ruleBlock->setConjunction(new Minimum);
	ruleBlock->setActivation(new Minimum);
	//Large Right
	ruleBlock->addRule(Rule::parse("if Displacement is LargeRight and Velocity is LargeRight then Steering is LargeLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is LargeRight and Velocity is MediumRight then Steering is LargeLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is LargeRight and Velocity is Zero then Steering is LargeLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is LargeRight and Velocity is MediumLeft then Steering is MediumLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is LargeRight and Velocity is LargeLeft then Steering is Zero", engine));
	
	//Medium Right
	ruleBlock->addRule(Rule::parse("if Displacement is MediumRight and Velocity is LargeRight then Steering is LargeLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is MediumRight and Velocity is MediumRight then Steering is LargeLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is MediumRight and Velocity is Zero then Steering is MediumLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is MediumRight and Velocity is MediumLeft then Steering is Zero", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is MediumRight and Velocity is LargeLeft then Steering is MediumRight", engine));

	//Centre
	ruleBlock->addRule(Rule::parse("if Displacement is Centre and Velocity is LargeRight then Steering is LargeLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is Centre and Velocity is MediumRight then Steering is MediumLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is Centre and Velocity is Zero then Steering is Zero", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is Centre and Velocity is MediumLeft then Steering is MediumRight", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is Centre and Velocity is LargeLeft then Steering is LargeRight", engine));

	//Medium Left
	ruleBlock->addRule(Rule::parse("if Displacement is MediumLeft and Velocity is LargeRight then Steering is MediumLeft", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is MediumLeft and Velocity is MediumRight then Steering is Zero", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is MediumLeft and Velocity is Zero then Steering is MediumRight", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is MediumLeft and Velocity is MediumLeft then Steering is LargeRight", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is MediumLeft and Velocity is LargeLeft then Steering is LargeRight", engine));

	//Large Left
	ruleBlock->addRule(Rule::parse("if Displacement is LargeLeft and Velocity is LargeRight then Steering is Zero", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is LargeLeft and Velocity is MediumRight then Steering is MediumRight", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is LargeLeft and Velocity is Zero then Steering is LargeRight", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is LargeLeft and Velocity is MediumLeft then Steering is LargeRight", engine));
	ruleBlock->addRule(Rule::parse("if Displacement is LargeLeft and Velocity is LargeLeft then Steering is LargeRight", engine));


	engine->addRuleBlock(ruleBlock);

	engine->configure("Minimum", "", "Minimum", "Maximum", "Centroid");

	std::string status;
	if (!engine->isReady(&status))
	{
		throw Exception("Engine not ready."
			"The following errors were encountered:\n" + status, FL_AT);
	}
	
	cout << "Welcome to Fuzzy Car Controller" << endl;

	while (true)
	{
		string sYesNo;
		string sAnswer;
		double disp = 0;
		double vel = 0;
		bool answer = false;
//		cout << "Do you want single value outputs? (Yes/No): ";
//		cin >> sYesNo;

		//if (sYesNo == "Yes" || sYesNo == "yes" || sYesNo == "YES" || sYesNo == "y")
	//	{
			cout << "Choose Displacement between -1 and 1 : ";
			cin >> disp;
			cout << "Choose a velocity between -1 and 1 : ";
			cin >> vel;
			Displacement->setInputValue((double)disp);
			Velocity->setInputValue((double)vel);
			engine->process();
			cout << "Steering is : " << (double)(Steering->getOutputValue()) << endl;
//		}

		cout << "Do you want to continue? (Yes/No): ";
		cin >> sYesNo;
		if (sYesNo == "Yes" || sYesNo == "yes" || sYesNo == "YES" || sYesNo == "y")
		{
			cout << "Do you want to display a complete set of outputs? (Yes/No): ";
			cin >> sYesNo;

			if (sYesNo == "Yes" || sYesNo == "yes" || sYesNo == "YES" || sYesNo == "y")
			{
				string sVelDisp;
				cout << "Would you like to hold Velocity of Displacement constant? (Vel/Disp): ";
				cin >> sVelDisp;
				if (sVelDisp == "disp")
				{
					double vel = 0;
					cout << "choose your velocity between -1 and 1: ";
					cin >> vel;
					for (int i = 0; i < 50; ++i)
					{
						double disp = Displacement->getMinimum() + i * (Displacement->range() / 50);
						Displacement->setInputValue(disp);
						Velocity->setInputValue(vel);
						engine->process();
						cout << "Displacement is : " << (double)(disp)
							<< " and velocity is : " << (double)(vel)
							<< " then Steering is :  " << (double)(Steering->getOutputValue()) << endl;
					}
				}
				else
				{
					double disp = 0;
					cout << "choose your displacement between -1 and 1: ";
					cin >> disp;
					for (int i = 0; i < 50; ++i)
					{
						double vel = Velocity->getMinimum() + i * (Velocity->range() / 50);
						Velocity->setInputValue(disp);
						Displacement->setInputValue(disp);
						engine->process();
						cout << "Displacement is : " << (double)(disp)
							<< " and velocity is : " << (double)(vel)
							<< " then Steering is :  " << (double)(Steering->getOutputValue()) << endl;
					}
				}
			}
			cout << endl;
		}
		else
		{
			return 0;
		}
	}
}
