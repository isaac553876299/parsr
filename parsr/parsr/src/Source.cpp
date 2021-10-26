#include "parsr.hpp"
int main()
{
	parsr_document test;
	while (1)
	{
		test.load("src/test.xml");
		system("pause");
		system("cls");
	}
	return 0;
}