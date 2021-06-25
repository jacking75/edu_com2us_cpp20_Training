#include <iostream>
#include <format>

int main()
{
	std::cout << std::format("{}, {}!", "Hello", "World") << std::endl;
	
	std::cout << std::format("{1}, {0}!", "World", "Hello") << std::endl;

	return 0;
}