#pragma once
#include <iostream>
#include <numeric>
#include "CSPData.h"
#include <ilcplex/ilocplex.h>

#define EPSILON 1e-6


class ColumnGeneration
{
public:
	ColumnGeneration(const CSPData& data);
	~ColumnGeneration();
private:
	IloEnv env;
	IloModel master;
	IloCplex cplex;
	CSPData data;
	std::vector<Pattern> patterns;
	std::vector<double> duals;
};

