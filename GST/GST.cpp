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
	// activePoint = (s, stringId, k, p) = s'
	if (k <= p) {
		char handle = this->strings[activeStringId][k];
		Reference handleTransition = s->getAdjacent(handle);

		Node *sPrime = handleTransition.child;
		int kPrime = handleTransition.u;
		int transitionStringId = handleTransition.stringId;
		int *pPrime = handleTransition.v;
		int offset = kPrime + p - k + 1;

		if (transitionStringId == -1) {
			printf("%d How???\n", s == this->root->getAdjacent('u').child);
		}

		if (this->strings[transitionStringId][offset] == t) {
			return std::make_pair(s, true);
		}

		Node* r = new Node(this->minusOnePointer);
		// g(s, (k', k' + v - u) = r
		// g(r, (k' + v - u + 1, p') = s'

		s->adjacent[handle] = Reference(r, transitionStringId, kPrime, new int(kPrime + p - k));
		// The right pointer for the new edge should remain the same, new pointer would invalidate our leaf update rule !!!!
		r->adjacent[this->strings[transitionStringId][kPrime + p - k + 1]] = Reference(sPrime, transitionStringId, kPrime + p - k + 1, pPrime);
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
			// Now do everything relative to activeStringId, should be ok verify once agin!!!
			handle = this->strings[activeStringId][k];
			next = s->getAdjacent(handle);
			sPrime = next.child;
			stringIdPrime = next.stringId;
			kPrime = next.u;
			pPrime = *(next.v);
		}
	}
	return ActivePoint(s, activeStringId, k);
}

ActivePoint GST::update(ActivePoint activePoint, int currentStringIndex, int i, int *leafPointer) {
	// activePoint = (active, stringId, k, i - 1) - cannonical reference to the active state
	// i - We are inserting this->strings[stringId][i] into the GST.
	Node *oldRoot = this->root;
	char newChar = this->strings[currentStringIndex][i];
	std::pair<Node*, bool> res = testAndSplit(activePoint.parrent, currentStringIndex, activePoint.u, i - 1, newChar);

	while (!res.second) {
		Leaf *leaf = new Leaf(this->minusOnePointer, currentStringIndex);
		// Encode the current string as the mask for the new leaf.
		res.first->adjacent[newChar] = Reference(leaf, currentStringIndex, i, leafPointer);
		
		if (oldRoot != root) {
			oldRoot->suffixLink = res.first;
		}

		oldRoot = res.first;
		activePoint = cannonize(activePoint.parrent->suffixLink, currentStringIndex, activePoint.u, i - 1);
		res = testAndSplit(activePoint.parrent, currentStringIndex, activePoint.u, i - 1, newChar);
	}

	if (oldRoot != this->root) {
		oldRoot->suffixLink = activePoint.parrent;
	}

	return activePoint;
}

// Implement walking down procedure for GST to get the new active state.
// We cannot use the skip/count trick here since we do not have any theoretical guarantees
// regarding the path label while descending the tree, which was not the case with 
// the build procecdure. Return also the label depth of the state that terminated the search, that should
// be used for GST queries.
std::pair<ActivePoint, std::pair<int, int>> GST::walkDown(std::string s) {
	ActivePoint activePoint(this->root, 0, 0);
	int p = -1;
	int edgeMatch = 0;
	char handle;
	int n = s.size();
	int firstI = 0;
	Node *child = this->root;
	int i = 0;

	// Label depth is always equal to i.

	while(i < n) {
		handle = s[i];
		// Start walking down from a new edge
		if(edgeMatch > p) {
			if(!child->containsEdge(handle)) {
				// Set k = i to signify that this state ends at a node! Otherwise testAndSplit would fail!
				// labelDepth = i, search terminated in a node.
				return std::make_pair(ActivePoint(child, activePoint.stringId, i), std::make_pair(i - 1, i));
			}

			Reference adjacent = child->getAdjacent(handle);
			activePoint.parrent = child;
			child = adjacent.child;
			activePoint.stringId = adjacent.stringId;
			activePoint.u = adjacent.u;
			p = *(adjacent.v);

			firstI = i;
			edgeMatch = adjacent.u;
		}

		else {
			if(this->strings[activePoint.stringId][edgeMatch] != handle) {
				// Remember the index value for the handle so that when edgeBreak happens
				// we return the index value for the handle to return the edge, returning i - 1 
				// causes seg. faluts!!!. 

				// Characters matched on the edge: [u, edgeMatch - 1]. Substract length of this from labelDepth.
				// i - (edgeMatch - 1 - u + 1) = i - edgeMatch - u
				return std::make_pair(ActivePoint(activePoint.parrent, activePoint.stringId, firstI), std::make_pair(i - 1, i - edgeMatch - activePoint.u));
			}

			edgeMatch++;
			i++;
		}
	}

	// The entire input string is contained in the tree.
	// Remaining part of the edge: [edgeMatch, p]. Add this to labelDepth.
	return std::make_pair(ActivePoint(child, activePoint.stringId, i - 1), std::make_pair(i - 1, i + p - edgeMatch + 1));
}

void GST::addString(std::string s) {
	if (lastTerminator == terminators.size()) {
		std::cout << "Maximum number of strings exceded." << std::endl;
		return;
	}

	// std::cout << "Adding string: " << s << std::endl;
	int stringIndex = this->strings.size();
	s += terminators[lastTerminator++];
	this->strings.push_back(s);
	std::pair<ActivePoint, std::pair<int, int>> wd = walkDown(s);

	ActivePoint activePoint = wd.first;

	int i = wd.second.first + 1;
	int *leafPointer = new int(i - 1);

	for(; i < s.size(); i++) {
		(*leafPointer)++;
		activePoint = update(activePoint, stringIndex, i, leafPointer);
		activePoint = cannonize(activePoint.parrent, stringIndex, activePoint.u, i);
	}
}

void GST::dfsPrivate(Node *current, std::vector<std::string> &buffer, std::map<int, std::vector<std::pair<std::string, int>>> &suffixes) {
	Leaf* ptr = dynamic_cast<Leaf*>(current);

	if (ptr != nullptr) {
		std::string suffix;
		for (auto it = buffer.begin(); it != buffer.end(); ++it) {
			suffix += *it;
			// std::cout << *it << " ";
		}

		// std::cout << std::endl;

		int suffixSize = suffix.size();
		suffixes[suffixSize].push_back(std::make_pair(suffix, ptr->stringId));
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
	std::map<int, std::vector<std::pair<std::string, int>>> suffixes; 
	dfsPrivate(this->root, buffer, suffixes);
	std::cout << "Suffixes:" << std::endl;

	for(auto it = suffixes.begin(); it != suffixes.end(); ++it) {
		for(auto j = it->second.begin(); j != it->second.end(); ++j) {
			std::cout << j->first << std::endl;
			std::cout << "String ID containing the suffix: " << j->second << std::endl;
		}
	}
}

std::unordered_map<int, std::vector<int>> GST::constructMap(Node* current, int labelDepth) {
	Leaf* leafTest = dynamic_cast<Leaf*>(current);
	std::unordered_map<int, std::vector<int>> res;

	if (leafTest != nullptr) {
		std::unordered_map<int, std::vector<int>> res;
		int n = this->strings[leafTest->stringId].size();
		res[leafTest->stringId] = std::vector<int>{ n - labelDepth };
		return res;
	}

	Node* child;
	int u, v;

	for (auto it = current->adjacent.begin(); it != current->adjacent.end(); ++it) {
		child = it->second.child;
		u = it->second.u;
		v = *(it->second.v);
		std::unordered_map<int, std::vector<int>> subRes = constructMap(child, labelDepth + v - u + 1);
		
		// at most k * z iterations, k is the number of strings and z is the maximal number of 
		// occurences of a pattern.
		for (auto i = subRes.begin(); i != subRes.end(); ++i) {
			for (auto j = i->second.begin(); j != i->second.end(); ++j) {
				res[i->first].push_back(*j);
			}
		}
	}
	return res;
}

bool GST::isSubstring(std::string query) {
	std::pair<ActivePoint, std::pair<int, int>> wd = walkDown(query);
	return wd.second.first == query.size() - 1;
}

bool GST::isSuffix(std::string query) {
	std::pair<ActivePoint, std::pair<int, int>> wd = walkDown(query);
	return wd.second.first == query.size() - 1 && (dynamic_cast<Leaf*>(wd.first.parrent) != nullptr);
}

std::unordered_map<int, std::vector<int>> GST::occurences(std::string query) {
	std::pair<ActivePoint, std::pair<int, int>> wd = walkDown(query);
	if (wd.second.first < query.size() - 1) {
		return std::unordered_map<int, std::vector<int>> {};
	}

	return constructMap(wd.first.parrent, wd.second.second);
}

std::unordered_map<int, std::vector<int>> GST::kCommonSubstringGeneralizedPrivate(Node *current, std::string pathLabel, 
	int minLength, int minStrings, int minOccurences, 
	std::vector<std::string> &solutions, 
	std::vector<std::unordered_map<int, std::vector<int>>> &occurences) {

	Leaf *leafTest = dynamic_cast<Leaf*>(current);
	std::unordered_map<int, std::vector<int>> res;

	if(leafTest != nullptr) {
		int n = this->strings[leafTest->stringId].size();
		res[leafTest->stringId] = std::vector<int>{ n - (int) (pathLabel.size()) };
		return res;
	}

	std::string descendString;
	std::unordered_map<int, std::vector<int>> subRes;
	Node* child;
	int childStringId, u, v;
	std::string lastChar;

	for(auto it = current->adjacent.begin(); it != current->adjacent.end(); ++it) {
		child = it->second.child;
		childStringId = it->second.stringId;
		u = it->second.u;
		v = *(it->second.v);
		descendString = pathLabel + this->strings[childStringId].substr(u, v - u + 1);
		subRes = kCommonSubstringGeneralizedPrivate(child, descendString, minLength, minStrings, minOccurences,
		solutions, occurences);

		for(auto i = subRes.begin(); i != subRes.end(); ++i) {
			for(auto j = i->second.begin(); j != i->second.end(); ++j) {
				res[i->first].push_back(*j);
			}
		}
	}

	if(pathLabel.size() >= minLength && res.size() >= minStrings) {
		bool isSolution = true;
		for(auto it = res.begin(); it != res.end(); ++it) {
			if(it->second.size() < minOccurences) {
				isSolution = false;
			}
		}

		if(isSolution) {
			solutions.push_back(pathLabel);
			occurences.push_back(res);
		}
	}

	return res;
}

void GST::kCommonSubstringGeneralized(int minLength, int minStrings, int minOccurences, std::vector<std::string> &solutions, std::vector<std::unordered_map<int, std::vector<int>>> &occurences) {
	kCommonSubstringGeneralizedPrivate(this->root, "", minLength, minStrings, minOccurences, solutions, occurences);	
}

int GST::longestCommonSubstringPrivate(Node *current, std::string pathLabel, std::string &solution, int supermask) {
	Leaf *leafTest = dynamic_cast<Leaf*>(current);
	if(leafTest != nullptr) {
		return 1 << (leafTest->stringId);
	}

	int currentMask = 0;
	Node *child;
	int childStringId, u, v;
	std::string descentString;

	for(auto it = current->adjacent.begin(); it != current->adjacent.end(); ++it) {
		child = it->second.child;
		childStringId = it->second.stringId;
		u = it->second.u;
		v = *(it->second.v);
		descentString = pathLabel + this->strings[childStringId].substr(u, v - u + 1);
		currentMask |= longestCommonSubstringPrivate(child, descentString, solution, supermask);
	}

	if(currentMask == supermask && pathLabel.size() > solution.size()) {
		solution = pathLabel;
	}

	return currentMask;
}

std::string GST::longestCommonSubstring() {
	std::string solution = "";
	int supermask = (1 << this->strings.size()) - 1;
	longestCommonSubstringPrivate(this->root, "", solution, supermask);
	return solution;
}