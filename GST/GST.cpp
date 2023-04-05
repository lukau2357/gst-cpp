#include "GST.h"
#include <iostream>

GST::GST() {
	this->minusOnePointer = new int(-1);
	this->zeroPointer = new int(0);
	this->root = new Node(this->minusOnePointer);
	this->sink = new SinkNode(this->root, this->zeroPointer);
	(this->root)->suffixLink = (Node*)(this->sink);
}

GST::~GST() {
	std::stack<Node*> inorderStack;
	inorderStack.push(this->root);
	// delete this->sink;
	Node* current;
	std::unordered_set<int*> pointerSet;

	while (!inorderStack.empty()) {
		current = inorderStack.top();
		inorderStack.pop();

		for (auto it = current->adjacent.begin(); it != current->adjacent.end(); ++it) {
			// Keep a set of pointers since they can be repeated, deleting one multiple times
			// causes seg. faults.
			pointerSet.insert(it->second.v);
			inorderStack.push(it->second.child);
		}

		delete current;
	}

	delete minusOnePointer;
	delete zeroPointer;

	for (auto it = pointerSet.begin(); it != pointerSet.end(); ++it) {
		delete *it;
	}
}

std::pair<Node*, bool>GST::testAndSplit(Node* s, int activeStringId, int k, int p, char t) {
	// activeState = (s, stringId, k, p) = s', k and p are relative to the shortest suffix of 
	// string on index stringId that passes through both s and s' in the tree.

	char handle = this->strings[activeStringId][k];

	if (k <= p) {
		// Could be problematic...
		// Input Reference needs to have parrent in parrent
		// Transitions from GST Nodes need to have next node in parrent!

		// Also problems could arise on bellow line!
		Reference handleTransition = s->getAdjacent(handle);

		// stringId of handleTransition should be the same as that of s?
		Node *sPrime = handleTransition.child;
		int kPrime = handleTransition.u;
		// int stringIdPrime = handleTransition.stringId; should be the same as activeStringId
		int *pPrime = handleTransition.v;
		int offset = kPrime + p - k + 1;

		if (this->strings[activeStringId][offset] == t) {
			return std::make_pair(s, true);
		}

		Node* r = new Node(this->minusOnePointer);
		// g(s, (k', k' + v - u) = r
		// g(r, (k' + v - u + 1, p') = s'
		// Virtualize adjacent function as well? For now not...
		
		s->adjacent[handle] = Reference(r, activeStringId, kPrime, new int(kPrime + p - k));
		// The right pointer for the new edge should remain the same, new would invalidate our leaf update rule !!!!
		r->adjacent[this->strings[activeStringId][kPrime + p - k + 1]] = Reference(sPrime, activeStringId, kPrime + p - k + 1, pPrime);

		// s->adjacent[handle] = Reference(r, activeStringId, k, new int(p));
		// r->adjacent[this->strings[activeStringId][p + 1]] = Reference(sPrime, activeStringId, p + 1, new int(pPrime));

		return std::make_pair(r, false);
	}

	if (s->containsEdge(t)) {
		return std::make_pair(s, true);
	}

	return std::make_pair(s, false);
}

// For a given reference pair r = g(s, stringId, k, p) return the cannonical reference pair
// g(s', stringId', k', p) for some state r. Note that pl(cannonical reference) is a suffix 
// of pl(reference) for every reference, so the right pointer does not change. 

// Add two activeStringIDs, one for k and another for p?
ActivePoint GST::cannonize(Node* s, int activeStringId, int k, int p) {
	if (k > p) {
		return ActivePoint(s, activeStringId, k);
	}

	char handle = this->strings[activeStringId][k];
	Reference next = s->getAdjacent(handle);

	Node* sPrime = next.child;
	int stringIdPrime = next.stringId;
	int kPrime = next.u;
	int pPrime = *(next.v);

	while (pPrime - kPrime <= p - k) {
		k += pPrime - kPrime + 1;
		s = sPrime;

		if (k <= p) {
			// Could be activeStringId instead of stringIdPrime?
			// Pay atention to this part when testing for GENERALIZED SUFFIX TREE!!!
			char handle = this->strings[stringIdPrime][k];
			next = s->getAdjacent(handle);
			sPrime = next.child;
			stringIdPrime = next.stringId;
			kPrime = next.u;
			pPrime = *(next.v);
		}
	}
	
	// Could be activeStringId instead of stringIdPrime?
	return ActivePoint(s, stringIdPrime, k);
}

ActivePoint GST::update(ActivePoint activePoint, int currentStringIndex, int i, int *leafPointer) {
	// activePoint = (active, stringId, k, i - 1) - cannonical reference to the active state
	// i - We are inserting this->strings[stringId][i] into the GST.

	Node *oldRoot = this->root;
	char newChar = this->strings[currentStringIndex][i];
	std::pair<Node*, bool> res = testAndSplit(activePoint.parrent, activePoint.stringId, activePoint.u, i - 1, newChar);

	while (!res.second) {
		// Leaf could have benn inserted before in GST, it could happend that multiple strings
		// Have a same suffix
		Leaf* leaf;
		if (!res.first->containsEdge(newChar)) {
			leaf = new Leaf(this->minusOnePointer);
			res.first->adjacent[newChar] =
				Reference((Node*)(leaf), currentStringIndex, i, leafPointer);
		}

		else {
			leaf = (Leaf*) res.first->getAdjacent(newChar).child;
		}

		// Not necessary to map the offset of the suffix for every string?
		leaf->matchingStrings.insert(currentStringIndex);
		
		if (oldRoot != root) {
			oldRoot->suffixLink = res.first;
		}

		oldRoot = res.first;
		activePoint = cannonize(activePoint.parrent->suffixLink, activePoint.stringId, activePoint.u, i - 1);
		res = testAndSplit(activePoint.parrent, activePoint.stringId, activePoint.u, i - 1, newChar);
	}

	if (oldRoot != this->root) {
		oldRoot->suffixLink = activePoint.parrent;
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
	ActivePoint activePoint(this->root, stringIndex, 0);

	for (int i = 0; i < s.size(); i++) {
		(*leafPointer)++;
		// printf("%p %d %d\n", activePoint.parrent, activePoint.stringId, activePoint.u);
		activePoint = update(activePoint, stringIndex, i, leafPointer);
		activePoint = cannonize(activePoint.parrent, activePoint.stringId, activePoint.u, i);
	}
}

void GST::dfsPrivate(Node *current, std::vector<std::string> &buffer, std::map<int, std::string> &suffixes) {
	Leaf* ptr = dynamic_cast<Leaf*>(current);

	if (ptr != nullptr) {
		std::string suffix;
		for (auto it = buffer.begin(); it != buffer.end(); ++it) {
			suffix += *it;
		}
		
		/*
		std::cout << "Suffix found: " << suffix << std::endl;
		std::cout << "Strings containing the given suffix: " << std::endl;
		for (auto it = ptr->matchingStrings.begin(); it != ptr->matchingStrings.end(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;*/

		int suffixSize = suffix.size();
		suffixes[suffixSize] = suffix;
		return;
	}

	for (auto it = current->adjacent.begin(); it != current->adjacent.end(); ++it) {
		Node* child = it->second.child;
		int stringId = it->second.stringId;
		int k = it->second.u;
		int p = *(it->second.v);
		buffer.push_back(this->strings[stringId].substr(k, p - k + 1));

		dfsPrivate(child, buffer, suffixes);
		buffer.pop_back();
	}
}

void GST::dfs() {
	std::vector<std::string> buffer;
	std::map<int, std::string> suffixes; 
	dfsPrivate(this->root, buffer, suffixes);
	std::cout << "Suffixes:" << std::endl;

	for(auto it = suffixes.begin(); it != suffixes.end(); ++it) {
		std::cout << it->second << std::endl;
	}
}
