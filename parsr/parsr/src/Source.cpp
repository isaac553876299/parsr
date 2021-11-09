#include "parsr.hpp"
int main()
{
	parsr_document test;
	while (1)
	{
		test.load("src/test.xml", true);
		system("pause");
		/*test.save("src/test.xml", true);
		system("pause");
		std::cout << test.root.child("subnode")->child("subsubnode")->attribute("three")->to_float() << std::endl;
		system("pause");*/
		system("cls");
	}
	return 0;
}