#include "parsr.hpp"
int main()
{
	parsr_document test;
	while (1)
	{
		test.load("src/test.xml");
		//system("pause");
		test.save("src/test.xml");
		//system("pause");
		//std::cout << stof(test.root.child("subnode")->attribute("two")->value) << std::endl;
		std::cout << test.root.child("subnode")->attribute("two")->value << std::endl;
		system("pause");
		system("cls");
	}
	return 0;
}