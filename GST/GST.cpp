#include "GST.h"
#include <iostream>

GST::GST() {
	this->root = new Node();
	this->sink = new SinkNode(root, this->minusOnePointer);
	(this->root)->suffixLink = (Node*)(this->sink);
	this->minusOnePointer = new int(-1);
}

GST::~GST() {
	std::stack<Node*> inorderStack;
	inorderStack.push(this->root);
	delete this->sink;
	Node* current;

	while (!inorderStack.empty()) {
		current = inorderStack.top();
		inorderStack.pop();

		for (auto it = current->adjacent.begin(); it != current->adjacent.end(); ++it) {
			// Delete the taken integer on the heap for the end point
			delete it->second.v;
			inorderStack.push(it->second.parrent);
		}

		delete current;
	}

	delete minusOnePointer;
}

std::pair<Node*, bool>GST::testAndSplit(Reference activeState, char t) {
	// activeState = (s, stringId, k, p) = s', k and p are relative to the shortest suffix of 
	// string on index stringId that passes through both s and s' in the tree.

	Node *parrent = activeState.parrent;
	int stringId = activeState.stringId;
	int u = activeState.u;
	int v = *(activeState.v);
	char handle = this->strings[stringId][u];

	if (u <= v) {
		// Could be problematic...
		// Input Reference needs to have parrent in parrent
		// Transitions from GST Nodes need to have next node in parrent!

		// Also problems could arise on bellow line!
		Reference handleTransition = parrent->getAdjacent(handle);

		// stringId of handleTransition should be the same as that of s?

		Node *sPrime = handleTransition.parrent;
		int kPrime = handleTransition.u;
		int stringIdPrime = handleTransition.stringId;
		int pPrime = *(handleTransition.v);
		int offset = kPrime + v - u + 1;

		if (this->strings[stringId][offset] == t) {
			return std::make_pair(parrent, true);
		}

		Node* r = new Node();
		// g(s, (k', k' + v - u) = r
		// g(r, (k' + v - u + 1, p') = s'
		parrent->adjacent[this->strings[stringId][kPrime]] = Reference(r, stringId, kPrime, new int(kPrime + v - u));
		r->adjacent[this->strings[stringId][kPrime + v - u + 1]] = Reference(sPrime, stringId, kPrime + v - u + 1, new int(pPrime));
		return std::make_pair(r, false);
	}

	if (parrent->adjacent.find(t) != parrent->adjacent.end()) {
		return std::make_pair(parrent, true);
	}

	return std::make_pair(parrent, false);
}

// For a given reference pair r = g(s, stringId, k, p) return the cannonical reference pair
// g(s', stringId', k', p) for some state r. Note that pl(cannonical reference) is a suffix 
// of pl(reference) for every reference, so the right pointer does not change. 
Reference GST::cannonize(Reference rp) {
	Node* s = rp.parrent;
	int stringId = rp.stringId;
	int k = rp.u;
	int p = *(rp.v);

	// std::cout << k << " " << p << std::endl;

	if (k > p) {
		return rp;
	}

	char handle = this->strings[stringId][k];
	Reference next = s->adjacent[handle];

	Node* sPrime = next.parrent;
	int stringIdPrime = next.stringId;
	int kPrime = next.u;
	int pPrime = *(next.v);

	while (pPrime - kPrime <= p - k) {
		k += pPrime - kPrime + 1;
		s = sPrime;

		if (k <= p) {
			char handle = this->strings[stringId][k];
			next = s->adjacent[handle];
			sPrime = next.parrent;
			stringIdPrime = next.stringId;
			kPrime = next.u;
			pPrime = *(next.v);
		}
	}

	return Reference(s, stringIdPrime, kPrime, rp.v);
}

Reference GST::update(Reference activePoint, int currentStringIndex, char newChar, int *leafPointer) {
	// activePoint = (active, stringId, k, i - 1) - cannonical reference to the active state
	// i - We are inserting this->strings[stringId][i] into the GST.

	Node *oldRoot = this->root;
	std::pair<Node*, bool> res = testAndSplit(activePoint, newChar);

	Node* activeParrent = activePoint.parrent;
	int activeStringId = activePoint.stringId;
	int activeU = activePoint.u;
	int* activeV = activePoint.v;

	while (!res.second) {
		// Leaf could have benn inserted before in GST, it could happend that multiple strings
		// Have a same suffix
		Leaf* leaf;
		if (res.first->adjacent.find(newChar) == res.first->adjacent.end()) {
			leaf = new Leaf();
			res.first->adjacent[newChar] =
				Reference((Node*)(leaf), currentStringIndex, *(activeV) + 1, leafPointer);
		}

		else {
			leaf = (Leaf*) res.first->adjacent[newChar].parrent;
		}

		// Not necessary to map the offset of the suffix for every string?
		leaf->suffixMap[currentStringIndex] = *(activeV) + 1;
		
		if (oldRoot != root) {
			oldRoot->suffixLink = res.first;
		}

		oldRoot = res.first;
		Reference cannonicalRP = cannonize(Reference(activeParrent->suffixLink, activeStringId, activeU, activeV));
		activePoint = cannonicalRP;
		activeParrent = activePoint.parrent;
		activeStringId = activePoint.stringId;
		activeU = activePoint.u;
		activeV = activePoint.v;
		res = testAndSplit(activePoint, newChar);
	}

	if (oldRoot != this->root) {
		oldRoot->suffixLink = res.first;
	}

	return activePoint;
}

void GST::addString(std::string s) {
	// Append terminator character to the local copy
	s += "$";
	int stringIndex = this->strings.size();
	this->strings.push_back(s);

	// For now single suffix implementation, no walking down!
	int* leafPointer = new int(-1);

	Reference activePoint(this->root, stringIndex, 0, minusOnePointer);

	for (int i = 0; i < s.size(); i++) {
		(*leafPointer)++;
		// std::cout << i << std::endl;
		activePoint = cannonize(activePoint);
		// printf("%d %d %d %d", activePoint.parrent == this->root, activePoint.stringId, activePoint.u, *activePoint.v);
		activePoint = update(activePoint, stringIndex, s[i], leafPointer);
	}
}

