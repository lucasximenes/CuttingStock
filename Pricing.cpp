#include "Pricing.h"

Pricing::Pricing(const IloEnv& env, const CSPData& data) : env(env), subProblem(env), data(data)
{
    subProblemSolver = IloCplex{ subProblem };
    auto len = data.orders.size();
	y = IloNumVarArray(env, len, 0, 20);
    
    IloExpr obj{ env };
    for (int i = 0; i < len; i++)
    {
        obj += 1.0 * y[i];
    }
    objective = IloMaximize(env, obj);
    subProblem.add(objective);
    obj.end();

	demandCons = IloRangeArray(env, len);
    for (int i = 0; i < len; i++)
    {
        IloExpr con{ env };
        con += data.orders[i].length * y[i];
        demandCons[i] = IloRange{ con <= data.capacity };
    }
}

std::pair<bool, Pattern> Pricing::Solve(IloNumArray duals)
{
    objective.setLinearCoefs(y, duals);
    subProblemSolver.solve();
    IloNum objVal = subProblemSolver.getObjValue();
    Pattern newPattern(data.orders.size(), 0);
    bool improved = false;
    if (objVal > 1.0 + EPSILON)
    {
        improved = true;
        std::cout << "New pattern found!\n";
        for (int i = 0; i < data.orders.size(); i++)
        {
			newPattern[i] = subProblemSolver.getValue(y[i]);
            std::cout << newPattern[i] << ' ';
		}
        std::cout << '\n';
	}
    return std::make_pair(improved, newPattern);
}