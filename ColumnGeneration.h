#pragma once
#include "Pricing.h"


class ColumnGeneration
{
public:
	ColumnGeneration(const CSPData& data);
	~ColumnGeneration();
	void Solve();
private:
	IloEnv env;
	IloModel master;
	IloCplex cplex;
	
	IloObjective objective;
	IloNumVarArray lambda;
	IloRangeArray cons;
	
	CSPData data;
	std::vector<Pattern> patterns;
	IloNumArray duals;

	Pricing pricing;
};

