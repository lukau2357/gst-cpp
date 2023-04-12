#include "Utils.h"

void test(std::vector<std::string> strings) {
	GST* tree = new GST();
	for (auto it = strings.begin(); it != strings.end(); ++it) {
		tree->addString(*it);
	}

	std::vector<std::string> buffer;
	std::map<int, std::vector<std::pair<std::string, int>>> suffixMap;
	std::unordered_map<std::string, int> repeatingSuffixMap;
	tree->dfsPrivate(tree->root, buffer, suffixMap);
	int errorCount = 0;

	// 1. Every path from the root to a leaf corrseponds to a suffix from one of the input strings
	// We will consider tree->strings because they have a terminator character appended. 
	// Also verify that paths from root to leaf are all unique.
	for (auto i = suffixMap.begin(); i != suffixMap.end(); ++i) {
		int suffixSize = i->first;
		for (auto j = i->second.begin(); j != i->second.end(); ++j) {
			std::string currentSuffix = j->first;
			int stringId = j->second;
			int n = tree->strings[stringId].size();

			if (tree->strings[stringId].substr(n - suffixSize, suffixSize) != currentSuffix) {
				std::cout << "Leaf path label " << currentSuffix << 
					" does not correspond to a suffix in string with id " << stringId << std::endl;
				++errorCount;
			}

			if(repeatingSuffixMap[currentSuffix] > 0) {
				std::cout << "Repating leaf path label " << currentSuffix 
						  << ". Already seen " << repeatingSuffixMap[currentSuffix]++ <<
							 " times." << std::endl;
			}
		}
 	}

	// 2. Every substring of every input string is contained in the tree. Furthermore, every suffix ends in a leaf.
	for (int k = 0; k < tree->strings.size(); k++) {
		std::string currentString = tree->strings[k];
		int n = currentString.size();
		for (int i = 0; i < n; i++) {
			for (int j = i; j < n; j++) {
				std::string current = currentString.substr(i, j - i + 1);
				// Suffix check
				if (j == n - 1) {
					if (!tree->isSuffix(current)) {
						std::cout << "Suffix " << current << " of input string with id " << k 
							<< " is not a suffix in the tree" << std::endl;
						++errorCount;
					}
				}

				// Substring check
				else {
					if (!tree->isSubstring(current)) {
						std::cout << "Substring " << current << " of input string with id " << k
							<< " is not a substring in the tree" << std::endl;
						++errorCount;
					}
				}
			}
		}
	}

	if (errorCount == 0) {
		std::cout << "GST correctly built!" << std::endl;
	}

	delete tree;
}

void generateAndTest(int n, int minLength, int maxLength) {
	// a -> 97
	// z -> 122
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> stringLength(minLength, maxLength);
	std::uniform_int_distribution<int> randomLetter(97, 122);
	std::vector<std::string> inStrings;

	for (int i = 0; i < n; i++) {
		int currentLength = stringLength(gen);

		while (currentLength < minLength) {
			currentLength = stringLength(gen);
		}

		std::string currentString;
		currentString.reserve(currentLength);

		for (int j = 0; j < currentLength; j++) {
			currentString += (char)(randomLetter(gen));
		}
		
		inStrings.push_back(currentString);
	}

	std::cout << "Generated strings: " << std::endl;

	for (int i = 0; i < n; i++) {
		std::cout << inStrings[i] << std::endl;
	}

	std::cout << std::endl;
	std::cout << "Testing for the given strings: " << std::endl;
	test(inStrings);
}