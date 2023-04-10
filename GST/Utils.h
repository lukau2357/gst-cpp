#ifndef UTILS_H
#define UTILS_H

#include "GST.h"
#include <iostream>
#include <random>

// Test if the GST constructed from the input strings is correct
void test(std::vector<std::string> strings);
// Generate n random strings and test if the tree is built correctly
void generateAndTest(int n, int minLength = 10, int maxLen = 100);

#endif
