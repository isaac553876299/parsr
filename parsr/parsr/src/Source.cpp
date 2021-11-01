#include "parsr.hpp"
int main()
{
	parsr_document test;
	while (1)
	{
		test.load("src/test.xml", true);
		system("pause");
		test.save("src/test.xml", true);
		system("pause");
		system("cls");
	}
	return 0;
}