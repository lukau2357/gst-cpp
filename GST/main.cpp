// GST.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
TODO:
	Instead of getting References from adjacent map use getAdjacent virtual function for sink node.
	Also implement a virtual function for checking wheter a node contains a given key.
	Should be OK after that...
*/

#include <iostream>
#include "GST.h"

int main() {
	GST* tree = new GST();
	tree->addString("test");
	delete tree;
	return 0;
}
