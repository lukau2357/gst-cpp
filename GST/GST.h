#ifndef GST_H
#define GST_H

#include <vector>
#include <stack>
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

	std::pair<Node*, bool> testAndSplit(Node *s, int activeStringId, int k, int p, char t);
	ActivePoint cannonize(Node *s, int activeStringId, int k, int p);
	std::pair<ActivePoint, int> walkDown(std::string s);
	// std::pair<ActivePoint, int> walkDown(std::string s);
	ActivePoint update(ActivePoint activePoint, int currentStringIndex, int i, int *leafPointer);

	// Test whether or not the given string is a suffix of one of the present build strings in the GST
	bool isSubstring(std::string query);
	bool isSuffix(std::string query);
	void addString(std::string s);
	void dfs();

	// For convenience, this remains a public method
	void dfsPrivate(Node *current, std::vector<std::string> &buffer, 
		std::map<int, std::vector<std::pair<std::string, int>>> &suffixes);
};

#endif