#include "Sequence.h"
#include <iostream>
#include <cassert>
using namespace std;

void test()
{
	Sequence s;
	assert(s.insert(0, 10));
	assert(s.insert(20));
	assert(s.size() == 2);
	ItemType x = 999;
	assert(s.get(0, x) && x == 10);
	assert(s.get(1, x) && x == 20);
}

int main()
{
	test();
	cout << "Passed all tests" << endl;
}