#ifndef NODE_H
#define NODE_H

// Use map instead of unordered_map to support lexicographical (inorder) traversal
// compute suffix arrays someday?
#include <unordered_map>
#include <bitset>

// Forward declaration of class Node required since Reference anc ActivePoint have pointers to Node
class Node;

class Reference {
public:
	Node *child;
	int stringId;
	int u;
	int* v;
	
	Reference() : child(nullptr), stringId(-1), u(-1), v(nullptr) {};
	Reference(int *invalidPointer) : child(nullptr), stringId(-1), u(0), v(invalidPointer) {};
	Reference(Node* child, int stringId, int u, int *v) : child(child), stringId(stringId), u(u), v(v) {};

	bool validReference();
};

// Represents the active point of the Ukkonen's algorithm
// (s, stringId, k), fourth index is reduntant since in i-th update it is always i - 1 for the active point!
class ActivePoint {
public:
	Node* parrent;
	int stringId;
	int u;

	ActivePoint() : parrent(nullptr), stringId(-1), u(-1) {};
	ActivePoint(Node* parrent, int stringId, int u) : parrent(parrent), stringId(stringId), u(u) {};
};

class Node {
public:
	Node* suffixLink;
	int* invalidPointer;
	// Mask which encodes the ids of the leaves below the current node.
	unsigned int mask = 0;

	Node(int *invalidPointer) : suffixLink(nullptr), invalidPointer(invalidPointer) {};
	virtual ~Node() {};
	std::unordered_map<char, Reference> adjacent;

	virtual bool containsEdge(char c);
	virtual Reference getAdjacent(char c);
};

class SinkNode : public Node {
public:
	Node* root;
	SinkNode(Node* root, int *invalidPointer) : Node(invalidPointer), root(root) {};
	~SinkNode() {};

	Reference getAdjacent(char c);
	bool containsEdge(char c);
};

class Leaf :public  Node {
public:
	int stringId;
	Leaf(int* invalidPointer, int stringId) : Node(invalidPointer), stringId(stringId) {};
	~Leaf() {};
};
#endif