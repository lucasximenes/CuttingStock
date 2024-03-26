#include "ColumnGeneration.h"


ColumnGeneration::ColumnGeneration(const CSPData& data) : data(data), pricing(env, data)
{
    master = IloModel(env);
    cplex = IloCplex(master);
    cplex.setOut(env.getNullStream());
    // Create initial patterns
    patterns = data.GenerateInitialPatterns();

    // Create the initial variables
    lambda = IloNumVarArray(env, patterns.size(), 0, IloInfinity);

    // Create the objective function
    IloExpr obj{ env };
    for (int i = 0; i < patterns.size(); i++)
    {
        obj += lambda[i];
    }
    objective = IloMinimize(env, obj);
    master.add(objective);
    obj.end();

    // Create the demand constraints
    cons = IloRangeArray(env, data.orders.size());
    for (int i = 0; i < data.orders.size(); i++)
    {
        IloExpr con{ env };
        for (int j = 0; j < patterns.size(); j++)
        {
            con += patterns[j][i] * lambda[j];
        }
        cons[i] = IloRange{ con >= data.orders[i].demand };
    }
    master.add(cons);
    duals = IloNumArray(env, data.orders.size());
}
ColumnGeneration::~ColumnGeneration()
{
    env.end();
}

void ColumnGeneration::Solve() 
{

    // Start column generation loop
    bool stop{ false };
    std::cout << "Starting column generation\n";
    int iter{ 0 };
    std::vector<double> dualAux(data.orders.size(), 0);
    while (!stop)
    {
        //cplex.exportModel("master.lp");
        // Solve the master problem
        cplex.solve();
        std::cout << "Iteration: " << iter++ << '\n';
        std::cout << "Objective: " << cplex.getObjValue() << '\n';

        // Get the dual values
        cplex.getDuals(duals, cons);
        for (int i = 0; i < data.orders.size(); i++)
        {
			dualAux[i] = duals[i];
		}
        std::pair<bool, Pattern> result{ pricing.Solve(duals) };
        if (result.first)
        {
            IloNumColumn col = objective(1.0);
            for (int i = 0; i < data.orders.size(); i++)
            {
				col += cons[i](result.second[i]);
			}
            lambda.add(IloNumVar(col, 0, IloInfinity, ILOFLOAT));
		}
        else
        {
			stop = true;
		}
    }
    std::cout << "Finished column generation\n";

}