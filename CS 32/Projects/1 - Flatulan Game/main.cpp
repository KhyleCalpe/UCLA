#include "City.h"
#include "History.h"
#include <iostream>
using namespace std;

int main()
{
	City c(2, 3);
	c.addPlayer(1, 2);
	c.addFlatulan(2, 1);
	c.addFlatulan(2, 1);
	c.addFlatulan(2, 3);
	while (c.flatulanCount() != 0)
		c.preachToFlatulansAroundPlayer();
	c.history().display();
	cout << "===" << endl;
}