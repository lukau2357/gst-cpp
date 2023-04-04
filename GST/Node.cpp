#include "Node.h"

bool Reference::validReference() {
	return this->child != nullptr;
}

Reference Node::getAdjacent(char c) {
	if (this->adjacent.find(c) == this->adjacent.end()) {
		return Reference(this->invalidPointer);
	}

	return this->adjacent[c];
}

bool Node::containsEdge(char c) {
	return this->getAdjacent(c).validReference();
}

Reference SinkNode::getAdjacent(char c) {
	// Instead of stringId being 0 pass it from GST functions?
	return Reference(this->root, 0, 0, this->invalidPointer);
}

bool SinkNode::containsEdge(char c) {
	return true;
}