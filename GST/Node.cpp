#include "Node.h"

bool Reference::emptyReference() {
	return this->u > *(this->v);
}

bool Reference::validReference() {
	return this->parrent != nullptr;
}

Reference Node::getAdjacent(char c) {
	if (this->adjacent.find(c) == this->adjacent.end()) {
		return Reference();
	}

	return this->adjacent[c];
}

Reference SinkNode::getAdjacent(char c) {
	return Reference(this->root, -1, 0, this->minusOnePointer);
}