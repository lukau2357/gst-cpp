#ifndef GST_H
#define GST_H

#include <vector>
#include <stack>
#include <unordered_set>
#include <map>
#include <string>
#include "Node.h"

class GST {
public:
	Node *root;
	SinkNode *sink;
	std::vector<std::string> strings;
	int *minusOnePointer, *zeroPointer;
	// Predefined set of terminator characters for each input string
	// This means that GST supports at most 8 addString operations, which is sufficient most of the time
	// in practice, especially in bioinformatics. One could extend this set further with more terminator characters,
	// or even implement the construction algorithm using only one terminator characater, but that would complicate
	// the update procedure.
	std::vector<std::string> terminators = { "$", "#", "!", "@", "%", "^", "&", "*" };
	int lastTerminator = 0; // Index of the last non-user terminator character.
	GST();
	~GST();

	// Functions for Ukkonen's algorithm
	std::pair<Node*, bool> testAndSplit(Node *s, int activeStringId, int k, int p, char t);
	ActivePoint cannonize(Node *s, int activeStringId, int k, int p);
	std::pair<ActivePoint, std::pair<int, int>> walkDown(std::string s);
	ActivePoint update(ActivePoint activePoint, int currentStringIndex, int i, int *leafPointer);
	void addString(std::string s);

	// Tree traversal functions
	// DFS functions for printing. Refactor to support general DFS traversal while applying an arbitrary
	// function to nodes during traversal.
	void dfs();
	// For convenience, this remains a public method
	void dfsPrivate(Node *current, std::vector<std::string> &buffer, 
		std::map<int, std::vector<std::pair<std::string, int>>> &suffixes);

	// Used to construct the following map for the sub-GST rooted at the current node
	// {stringId -> vector of all sufixes of the given string below the current node}
	// cumLength is the label depth of the given node.
	std::unordered_map<int, std::vector<int>> constructMap(Node* current, int cumLength);

	// GST queries
	bool isSubstring(std::string query);
	bool isSuffix(std::string query);
	// Return all string Ids that contain the query as a substring.
	std::unordered_map<int, std::vector<int>> occurences(std::string query); 
	// DFS traversal function for the generalized k common substring problem. We want all k common substrings that 
	// are of length at least minLength, and they occur at least minOccurences times in every string they appear.
	// Should also be a private function but made public for testing convenience.
	std::unordered_map<int, std::vector<int>> kCommonSubstringGeneralizedPrivate(Node* current, std::string pathLabel,
		int minLength, int minStrings, int minOccurences, 
		std::vector<std::string> &solutions, 
		std::vector<std::unordered_map<int, std::vector<int>>> &occurences);
	void kCommonSubstringGeneralized(int minLength, int minStrings, int minOccurences, std::vector<std::string> &solutions, std::vector<std::unordered_map<int, std::vector<int>>> &occurences);
	// Functions which solve the longestCommonSubstring problem. Unlike the other functions, we return only the longest common
	// substring without any other restrictions, and this can be done in linear time. 
	// supermask refers to a bitmask which encodes every string present in the GST, it is precomputed and passed as
	// a parameter to this function.
	int longestCommonSubstringPrivate(Node *current, std::string pathLabel, std::string &solution, int supermask);
	std::string longestCommonSubstring();
};

#endif