// GST.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <bitset>

#include "GST.h"
#include "Utils.h"

int main() {
	GST* tree = new GST();
	std::vector<std::string> strings = { "abbbbbbbbbb", "abxb", "xbd" };

	for (auto it = strings.begin(); it != strings.end(); ++it) {
		tree->addString(*it);
	}

	std::unordered_map<int, std::vector<int>> occurences = tree->occurences("b");
	for (auto i = occurences.begin(); i != occurences.end(); ++i) {
		for (auto j = i->second.begin(); j != i->second.end(); ++j) {
			std::cout << i->first << " " << *j << std::endl;
		}
	}

	delete tree;
	// generateAndTest(8);
	return 0;
}