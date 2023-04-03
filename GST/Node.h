#ifndef NODE_H
#define NODE_H

// Use map instead of unordered_map to support lexicographical (inorder) traversal
// compute suffix arrays someday?
#include <unordered_map>

// Convenience variables since right end is always kept as a pointer
// to solve extension 1 <= h < j, where j is the index of the active suffix
// https://stackoverflow.com/questions/15517658/fatal-error-lnk1169-one-or-more-multiply-defined-symbols-found-in-game-programm

// For every Reference pair we create an integer on the heap for the p pointer.
// This adds a 4-byte memory overhead however it allows us to solve leaf updates
// in constant time (by incrementing the pointer of all leaves), so that is good.

// Forward declaration
class Reference;

class Node {
public:
	Node* suffixLink;
	Node() : suffixLink(nullptr) {};
	virtual ~Node() {};
	Node(Node* suffixLink) : suffixLink(suffixLink) {};
	std::unordered_map<char, Reference> adjacent;

	virtual Reference getAdjacent(char c);
};

class Reference {
public:
	Node* parrent;
	int stringId;
	int u;
	int* v;

	// Memory leak possible!
	Reference() : parrent(nullptr), stringId(-1), u(0), v(new int(-1)) {};
	Reference(Node* parrent, int stringId, int u, int* v) : parrent(parrent), stringId(stringId), u(u), v(v) {};

	bool emptyReference();
	bool validReference();
};

class SinkNode : Node {
public:
	Node* root;
	int* minusOnePointer;
	SinkNode() : Node(), root(nullptr), minusOnePointer(nullptr) {};
	SinkNode(Node* root, int *minusOnePointer) : Node(), root(root), minusOnePointer(minusOnePointer) {};
	~SinkNode() {};
	Reference getAdjacent(char c);
};

class Leaf : Node {
public:
	// In GST one leaf can represent multiple suffixes, keep track for each different input string
	// The id of the suffix that it represents
	std::unordered_map<int, int> suffixMap;
	~Leaf() {};
};

#endif