#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#define EPSILON 1e-4


using Pattern = std::vector<int>;

struct Order {
	int length{};
	int demand{};
};

class CSPData
{
public:
	int capacity;
	std::vector<Order> orders;

	CSPData(int capacity, std::vector<Order> orders) : capacity(capacity), orders(orders) {}
	CSPData(std::string filename);
	std::vector<Pattern> GenerateInitialPatterns() const;
};