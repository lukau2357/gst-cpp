#include <chrono>
#include <iostream>
#include <unordered_map>
#include <bitset>
#include <vector>

#include "GST.h"
#include "Utils.h"

using namespace std;

int main() {
	
	vector<string> strings {"bsfdgdsffdgsdfg", "ffdsffsd"};
	GST *tree = new GST();

	for(auto it = strings.begin(); it != strings.end(); ++it) {
		tree->addString(*it);
	}

	// LCS query
	/** string lcs = tree->longestCommonSubstring();
	cout << lcs << endl; **/	

	// k common substring generalized query
	/** vector<string> solutions;
	vector<unordered_map<int, vector<int>>> occurrences;
	tree->kCommonSubstringGeneralized(2, 2, 1, solutions, occurrences);

	for(int i = 0; i < solutions.size(); i++) {
		cout << "Substring that satisfies the constraints: " << solutions[i] << endl;
		for(auto it = occurrences[i].begin(); it != occurrences[i].end(); ++it) {
			cout << "Occurrences in " << tree->strings[it->first] << endl;
			for(auto occ = it->second.begin(); occ != it->second.end(); ++occ) {
				cout << *occ << " ";
			}
			cout << endl;
			cout << endl;
		}
	} **/

	// is substring query, substring of at least one of the input strings that is
	/** string pattern = "dfgg";
	cout << tree->isSubstring(pattern) << endl; **/

	// is suffix query, suffix if at least one of the input strings that is 
	/** string pattern = "dfgg";
	cout << tree->isSuffix(pattern) << endl; **/

	// All occurrences in all input strings of the given pattern
	/*string pattern = "ffd";
	unordered_map<int, vector<int>> occurrences = tree->occurences(pattern);
	cout << "Occurrences of the given pattern in tree strings: " << endl;
	for(int i = 0; i < occurrences.size(); i++) {
		cout << "Occurrences in " << tree->strings[i] << endl;
		for(auto it = occurrences[i].begin(); it != occurrences[i].end(); ++it) {
			cout << *it << " ";
		}
		cout << endl;
		cout << endl;
	}*/

	// Testing function
	generateAndTest(8);
	delete tree;
	return 0;
}