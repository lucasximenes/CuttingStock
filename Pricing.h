#pragma once
#include "CSPData.h"
#include <ilcplex/ilocplex.h>

class Pricing
{
public:
	Pricing(const IloEnv& env, const CSPData& data);
	std::pair<bool, Pattern> Solve(IloNumArray duals);
private:
	CSPData data;
	
	IloEnv env;
	IloCplex subProblemSolver;
	IloModel subProblem;
	
	IloNumVarArray y;
	IloObjective objective;
};

