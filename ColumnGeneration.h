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
	void Solve();
	std::vector<Pattern> PricingSubproblem();
private:
	IloEnv env;
	IloModel master;
	IloCplex cplex;
	
	IloNumVarArray lambda;
	IloRangeArray cons;
	
	CSPData data;
	std::vector<Pattern> patterns;
	std::vector<double> duals;
};

