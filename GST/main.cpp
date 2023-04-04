// GST.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
TODO:
	Instead of getting References from adjacent map use getAdjacent virtual function for sink node.
	Also implement a virtual function for checking wheter a node contains a given key.
	Should be OK after that...
*/

#include <chrono>
#include <iostream>
#include "GST.h"

int main() {
	GST* tree = new GST();
	auto start = std::chrono::high_resolution_clock::now();
	tree->addString("abcdd");
	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Time required to build the tree: "
		<<
	std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
		<< std::endl;

	// tree->dfs();

	delete tree;

	/*int invalidPointer = -1;
	Node* test = new Leaf(&invalidPointer);
	
	if (dynamic_cast<Leaf*>(test)) {
		std::cout << "yes" << std::endl;
	}*/

	return 0;
}
