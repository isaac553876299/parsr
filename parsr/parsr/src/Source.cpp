#include "parsr.hpp"
#include <chrono>
int main()
{
	parsr_document doc;
	while (1)
	{
		auto myclock = std::chrono::high_resolution_clock::now();
		std::cout << "\033[4mload\033[0m " << (doc.load("src/test_load.xml") ? "\x1B[32mwell_formed\033[0m" : "\x1B[31mill_formed\033[0m") << std::endl;
		std::cout << "//seconds: " << (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - myclock)).count() << std::endl;

		std::cout << "\033[4msave\033[0m " << (doc.save("src/test_save.xml") ? "\x1B[32mO\033[0m" : "\x1B[31mX\033[0m") << std::endl;

		std::cout << "\033[4mdocument\033[0m\n" << doc << std::endl;

		//std::cout << test.root.child("subnode")->attribute("two")->value << std::endl;
		system("pause");
		system("cls");
	}
	return 0;
}