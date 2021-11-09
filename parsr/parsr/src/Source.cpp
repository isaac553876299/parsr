#include "parsr.hpp"
int main()
{
	parsr_document test;
	test.load("src/test.xml", true);
	//system("pause");
	test.save("src/test.xml", true);
	//system("pause");
	std::cout << test.root.child("subnode")->attribute("two")->to_float() << std::endl;

	return 0;
}