// GST.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <chrono>
#include <iostream>
#include <map>
#include <unordered_map>

#include "GST.h"
#include "Utils.h"

int main() {
	/* GST* tree = new GST();
	for (auto it = inStrings.begin(); it != inStrings.end(); ++it) {
		tree->addString(*it);
	}

	tree->dfs();
	delete tree;*/
	// test(inStrings);
	
	generateAndTest(8);
	return 0;
}