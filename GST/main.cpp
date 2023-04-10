// GST.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <chrono>
#include <iostream>
#include <map>
#include <unordered_map>

#include "GST.h"
#include "Utils.h"

int main() {
	GST* tree = new GST();
	std::vector<std::string> inStrings = { "mississipi", "crawthipi", "a" };
	// test(inStrings);

	for (int i = 0; i < inStrings.size(); i++) {
		tree->addString(inStrings[i]);
	}

	tree->dfs();
	delete tree;

	return 0;
}