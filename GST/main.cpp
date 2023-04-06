// GST.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <chrono>
#include <iostream>
#include <map>
#include <unordered_map>
#include "GST.h"

int main() {
	GST* tree = new GST();
	std::string inString = "DFGJDFGKJDFGLKJDFGKJD";

	// auto start = std::chrono::high_resolution_clock::now();
	tree->addString(inString);
	// auto end = std::chrono::high_resolution_clock::now();

	/*std::cout << "Time required to build the tree: "
		<<
	std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
		<< std::endl;*/

	std::string testString = "testKJD";
	tree->addString(testString);
	tree->dfs();

	// std::string suffixTest = "GJKFDG";
	// printf("%d\n", tree->isSuffix(suffixTest));

	delete tree;
	return 0;
}