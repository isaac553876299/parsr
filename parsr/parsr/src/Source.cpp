#include "parsr.hpp"
#include <chrono>
int main()
{
	parsr_document doc;
	while (1)
	{
		auto myclock = std::chrono::high_resolution_clock::now();
		doc.load("src/test.xml");
		std::cout << "//seconds: " << (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - myclock)).count() << std::endl;

		doc.save("src/test.xml");

		std::cout << doc << std::endl;

		//std::cout << test.root.child("subnode")->attribute("two")->value << std::endl;
		system("pause");
		system("cls");
	}
	return 0;
}