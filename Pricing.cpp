#include "Pricing.h"

Pricing::Pricing(const IloEnv& env, const CSPData& data) : env(env), subProblem(env), data(data)
{
    subProblemSolver = IloCplex{ subProblem };
    subProblemSolver.setOut(env.getNullStream());
    auto len = data.orders.size();
    y = IloNumVarArray(env, len, 0, 20, ILOINT);
    
    IloExpr obj{ env };
    for (int i = 0; i < len; i++)
    {
        obj += 1.0 * y[i];
    }
    z = IloMaximize(env, obj);
    subProblem.add(z);
    obj.end();

    IloExpr con{ env };
    for (int i = 0; i < len; i++)
    {
        con += data.orders[i].length * y[i];
    }
    IloRange capacityCon{ con <= data.capacity };
    subProblem.add(capacityCon);
}

std::pair<bool, Pattern> Pricing::Solve(IloNumArray duals)
{
    z.setLinearCoefs(y, duals);
    //subProblemSolver.exportModel("subProblem.lp");
    subProblemSolver.solve();
    IloNum objVal = subProblemSolver.getObjValue();
    Pattern newPattern(data.orders.size(), 0);
    bool improved = false;
    if (objVal > 1.0 + EPSILON)
    {
        improved = true;
        std::cout << "New pattern found!\n";
        double val = 0;
        for (int i = 0; i < data.orders.size(); i++)
        {
            val = subProblemSolver.getValue(y[i]);
            if (val > EPSILON)
            {
                std::cout << "Order " << i << ": " << val << " || ";
                newPattern[i] = (int)(ceil(val - EPSILON) + 0.5);
            }
		}
        std::cout << '\n';
	}
    return std::make_pair(improved, newPattern);
}