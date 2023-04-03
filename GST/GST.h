#ifndef GST_H
#define GST_H

#include <vector>
#include <stack>
#include "Node.h"

class GST {
public:
	Node *root;
	SinkNode *sink;
	std::vector<std::string> strings;
	int *minusOnePointer;

	GST();
	~GST();

	std::pair<Node*, bool> testAndSplit(Reference s, char t);
	Reference cannonize(Reference rp);
	Reference update(Reference activePoint, int currentStringIndex, char newChar, int *leafPointer);
	void addString(std::string s);
};

#endif

