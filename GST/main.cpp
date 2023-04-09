// GST.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <chrono>
#include <iostream>
#include <map>
#include <unordered_map>
#include "GST.h"

int main() {
	GST* tree = new GST();
	// auto start = std::chrono::high_resolution_clock::now();
	// auto end = std::chrono::high_resolution_clock::now();

	/*std::cout << "Time required to build the tree: "
		<<
	std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
		<< std::endl;*/

	std::vector<std::string> inStrings = { "mississipi", "missouripi" };
	int cumLength = 0;
	for (auto s = inStrings.begin(); s != inStrings.end(); ++s) {
		tree->addString(*s);
		cumLength += s->size() + 1;
	}

	std::string currentString;
	tree->dfs();
	printf("%d %d\n", tree->leafCounter, cumLength);
	// printf("%d\n", tree->isSubstring("ipiz"));
	// printf("%d\n", tree->isSuffix("ii$")); 
	delete tree;
	return 0;
}