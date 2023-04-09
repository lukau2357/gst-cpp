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
	// activeState = (s, stringId, k, p) = s'
	// activeStringId treba da bude id stringa koji se unosi!

	if (k <= p) {
		char handle = this->strings[activeStringId][k];
		// Could be problematic...
		// Input Reference needs to have parrent in parrent
		// Transitions from GST Nodes need to have next node in parrent!
		// Also problems could arise on bellow line!
		Reference handleTransition = s->getAdjacent(handle);

		// stringId of handleTransition should be the same as that of s?
		Node *sPrime = handleTransition.child;
		int kPrime = handleTransition.u;
		int transitionStringId = handleTransition.stringId;
		int *pPrime = handleTransition.v;
		int offset = kPrime + p - k + 1;

		// getting from activeStringId before...
		if (this->strings[transitionStringId][offset] == t) {
			return std::make_pair(s, true);
		}

		Node* r = new Node(this->minusOnePointer);
		// g(s, (k', k' + v - u) = r
		// g(r, (k' + v - u + 1, p') = s'
		// Virtualize adjacent function as well? For now not...
		s->adjacent[handle] = Reference(r, transitionStringId, kPrime, new int(kPrime + p - k));
		// The right pointer for the new edge should remain the same, new would invalidate our leaf update rule !!!!
		r->adjacent[this->strings[transitionStringId][kPrime + p - k + 1]] = Reference(sPrime, transitionStringId, kPrime + p - k + 1, pPrime);
		return std::make_pair(r, false);
	}

	if (s->containsEdge(t)) {
		/*Leaf* leafTest = dynamic_cast<Leaf*>(s->getAdjacent(t).child);
		if (dynamic_cast<Leaf*>(leafTest) != nullptr) {
			leafTest->matchingStrings.insert(activeStringId);
			printf("Testing for leaf: %d\n", leafTest->suffixLink != nullptr);
		}*/
		return std::make_pair(s, true);
	}

	return std::make_pair(s, false);
}

// For a given reference pair r = g(s, stringId, k, p) return the cannonical reference pair
// g(s', stringId', k', p) for some state r. Note that pl(cannonical reference) is a suffix 
// of pl(reference) for every reference, so the right pointer does not change. 
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
			// handle lookup always relative to activeStringId?
			// Now do everything relative to activeStringId, should be ok verify once agin!!!
			handle = this->strings[activeStringId][k];
			next = s->getAdjacent(handle);
			sPrime = next.child;
			stringIdPrime = next.stringId;
			kPrime = next.u;
			pPrime = *(next.v);
		}
	}
	
	// Could be activeStringId instead of stringIdPrime?
	// For GST it turns out that settings stringIdPrime here instead of activeStringId was a mistake, investiagte!
	return ActivePoint(s, activeStringId, k);
}

ActivePoint GST::update(ActivePoint activePoint, int currentStringIndex, int i, int *leafPointer) {
	// activePoint = (active, stringId, k, i - 1) - cannonical reference to the active state
	// i - We are inserting this->strings[stringId][i] into the GST.

	Node *oldRoot = this->root;
	char newChar = this->strings[currentStringIndex][i];
	// Bilo je activePoint.stringId!!!
	std::pair<Node*, bool> res = testAndSplit(activePoint.parrent, currentStringIndex, activePoint.u, i - 1, newChar);

	while (!res.second) {
		// Leaf could have benn inserted before in GST, it could happend that multiple strings
		// Have a same suffix

		/*
		Leaf* leaf;
		if (!res.first->containsEdge(newChar)) {
			leaf = new Leaf(this->minusOnePointer);
			res.first->adjacent[newChar] = Reference(leaf, currentStringIndex, i, leafPointer);
		}

		else {
			leaf = (Leaf*)(res.first->getAdjacent(newChar).child);
		}

		leaf->matchingStrings.insert(currentStringIndex);*/

		Leaf *leaf = new Leaf(this->minusOnePointer);
		res.first->adjacent[newChar] = Reference(leaf, currentStringIndex, i, leafPointer);
		leaf->matchingStrings.insert(currentStringIndex);
		
		if (oldRoot != root) {
			oldRoot->suffixLink = res.first;
		}

		oldRoot = res.first;
		// Bilo je activePoint.stringId
		activePoint = cannonize(activePoint.parrent->suffixLink, currentStringIndex, activePoint.u, i - 1);
		// Bilo je activePoint.stringId
		res = testAndSplit(activePoint.parrent, currentStringIndex, activePoint.u, i - 1, newChar);
	}

	if (oldRoot != this->root) {
		oldRoot->suffixLink = activePoint.parrent;
	}

	// If terminator (assumed to always be $) is the current character, extend potentially previously
	// addedd suffixes.

	// res je cvor na koje treba nakaciti list
	if (newChar == '$') {

	}

	return activePoint;
}

// Implement walking down procedure for GST to get the new active state.
// We cannot use the skip/count trick here since we do not have any theoretical guarantees
// regarding the path label while descending the tree, which was not the case with 
// the build procecdure.
std::pair<ActivePoint, int> GST::walkDown(std::string s) {
	ActivePoint activePoint(this->root, 0, 0);
	int p = -1;
	int edgeMatch = 0;
	char handle;
	int n = s.size();
	Node *child = this->root;
	int i = 0;

	for(; i < n;) {
		handle = s[i];
		// Start walking down from a new edge
		if(edgeMatch > p) {
			if(!child->containsEdge(handle)) {
				return std::make_pair(ActivePoint(child, activePoint.stringId, activePoint.u), i - 1);
			}

			Reference adjacent = child->getAdjacent(handle);
			activePoint.parrent = child;
			child = adjacent.child;
			activePoint.stringId = adjacent.stringId;
			activePoint.u = adjacent.u;
			p = *(adjacent.v);
			edgeMatch = adjacent.u;
		}

		else {
			if(this->strings[activePoint.stringId][edgeMatch] != handle) {
				std::cout << s.substr(0, i + 1) << std::endl;
				return std::make_pair(ActivePoint(activePoint.parrent, activePoint.stringId, activePoint.u), i - 1);
			}

			edgeMatch++;
			i++;
		}
	}

	// The entire input string is contained in the tree.
	return std::make_pair(ActivePoint(child, activePoint.stringId, activePoint.u), i);
}

void GST::addString(std::string s) {
	// Append terminator character to the local copy
	s += "$";
	int stringIndex = this->strings.size();
	this->strings.push_back(s);

	std::pair<ActivePoint, int> wd = walkDown(s);
	ActivePoint activePoint = wd.first;
	int i = wd.second + 1;
	int *leafPointer = new int(i - 1);

	for(; i < s.size(); i++) {
		printf("%d\n", i);
		(*leafPointer)++;
		activePoint = update(activePoint, stringIndex, i, leafPointer);
		// bilo je activePoint.stringId
		activePoint = cannonize(activePoint.parrent, stringIndex, activePoint.u, i);
	}
}

void GST::dfsPrivate(Node *current, std::vector<std::string> &buffer, std::map<int, std::vector<std::pair<std::string, std::unordered_set<int>>>> &suffixes) {
	Leaf* ptr = dynamic_cast<Leaf*>(current);

	if (ptr != nullptr) {
		std::string suffix;
		for (auto it = buffer.begin(); it != buffer.end(); ++it) {
			// std::cout << *it << " ";
			suffix += *it;
		}

		// std::cout <<  std::endl;

		int suffixSize = suffix.size();
		suffixes[suffixSize].push_back(std::make_pair(suffix, ptr->matchingStrings));
		this->leafCounter++;
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
	std::map<int, std::vector<std::pair<std::string, std::unordered_set<int>>>> suffixes; 
	dfsPrivate(this->root, buffer, suffixes);
	std::cout << "Suffixes:" << std::endl;

	for(auto it = suffixes.begin(); it != suffixes.end(); ++it) {
		for(auto j = it->second.begin(); j != it->second.end(); ++j) {
			std::cout << j->first << std::endl;
			std::cout << "String IDs containing the suffix:" << std::endl;
			for(auto k = j->second.begin(); k != j->second.end(); ++k) {
				std::cout << *k << " ";
			}
			std::cout << std::endl;
		}
	}
}

bool GST::isSubstring(std::string query) {
	std::pair<ActivePoint, int> wd = walkDown(query);
	return wd.second == query.size();
}

bool GST::isSuffix(std::string query) {
	std::pair<ActivePoint, int> wd = walkDown(query);
	return wd.second == query.size() && (dynamic_cast<Leaf*>(wd.first.parrent) != nullptr);
}