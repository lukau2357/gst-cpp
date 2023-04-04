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

	GST();
	~GST();

	std::pair<Node*, bool> testAndSplit(Node *s, int activeStringId, int k, int p, char t);
	ActivePoint cannonize(Node *s, int activeStringId, int k, int p);
	ActivePoint update(ActivePoint activePoint, int currentStringIndex, int i, int *leafPointer);
	void addString(std::string s);
	void dfs();
private:
	// Private recursive traversal of GST, use a vector to accumulate strings along visited edges
	void dfsPrivate(Node* current, std::vector<std::string>& buffer);
};

#endif