#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include "SerialMC.h"

struct Command
{
	double left;
	double right;
	double seconds;
};

std::vector<Command> parse_args(std::vector<std::string> const& args)
{
	if (args.size() == 0)
	{
		std::cerr << "No arguments, try --help" << std::endl;
	}
	else if (args[0] == "-h" || args[0] == "--help")
	{
		std::cout << "Run with sets of three arguments, left right and time (in seconds)" << std::endl;
		std::cout << "For example:" << std::endl;
		std::cout << "   ./run-motors 1 0.5 1.6 -1 -1 2" << std::endl;
		std::cout << "Would run the robot forward with the right motor at half power for 1.6 seconds," << std::endl;
		std::cout << "then full reverse for 2 seconds" << std::endl;
		exit(0);
	}
	else if (args.size() % 3 != 0)
	{
		std::cerr << "Argument count not divisable by 3" << std::endl;
	}
	else
	{
		std::vector<Command> result;
		for (unsigned i = 0; i < args.size(); i += 3)
		{
			result.push_back(Command{
				std::stod(args[i]),
				std::stod(args[i + 1]),
				std::stod(args[i + 2])});
			if (result.back().seconds <= 0)
			{
				std::cerr << "Seconds should be >= 0" << std::endl;
			}
		}
		return result;
	}
	exit(1);
}

int main(int argc, char** argv)
{
	std::vector<std::string> args;
	for (int i = 1; i < argc; i++)
	{
		args.push_back(argv[i]);
	}
	auto const commands = parse_args(args);
	SerialMC mc;
	for (auto const& command : commands)
	{
		mc.set(command.left, command.right);
		usleep(command.seconds * 1000000);
	}
	return 0;
}
