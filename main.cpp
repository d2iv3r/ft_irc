#include "./Includes/server.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3) {
		std::cout << "Error: Invalid arguments!\n";
		return (1);
	}
	try
	{
		Server irc(argv[1], argv[2]);
		irc.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
