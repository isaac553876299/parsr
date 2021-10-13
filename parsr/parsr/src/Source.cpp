#include "parsr.hpp"
int main()
{
	const char arr[] = "-\\|/";
	int x = 0, y = 0;
	while (0)
	{
		++y;
		if (y > 99999999)
		{
			y = 0;
			std::cout << '\b' << arr[(x > sizeof(arr) - 1) ? x = 0 : x++];
		}
	}
	parsr_document test("src/test.xml");
	return 0;
}