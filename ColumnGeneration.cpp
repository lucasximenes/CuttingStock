#include "ColumnGeneration.h"


ColumnGeneration::ColumnGeneration(const CSPData& data)
{
    
    master = IloModel(env);
    cplex = IloCplex(master);
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
    master.add(IloMinimize(env, obj));
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
}
ColumnGeneration::~ColumnGeneration()
{
    env.end();
}

std::vector<Pattern> ColumnGeneration::PricingSubproblem()
{
    IloModel subModel{ env };
    IloIntVarArray y{ env, duals.size(), 0, 20 };
    IloExpr obj{ env };
    for (int i = 0; i < duals.size(); i++)
    {
        obj += duals[i] * y[i];
    }
    IloRangeArray demandCons{ env, data.orders.size() };
    for (int i = 0; i < data.orders.size(); i++)
    {
        IloExpr con{ env };
        for (int j = 0; j < duals.size(); j++)
        {
            con += patterns[j][i] * y[j];
        }
        demandCons[i] = IloRange{ con >= data.orders[i].demand };
    }


    return {};
}

void ColumnGeneration::Solve() 
{

    // Start column generation loop
    bool stop{ false };
    while (!stop)
    {
        // Solve the master problem
        cplex.solve();
        std::cout << "Objective: " << cplex.getObjValue() << '\n';

        // Get the dual values
        IloNumArray temp{ env };
        std::vector<double> duals(lambda.getSize());
        cplex.getDuals(temp, cons);
        for (int i = 0; i < duals.size(); i++)
        {
            duals[i] = temp[i];
        }

        bool hasNegativeDual = std::accumulate(duals.begin(), duals.end(), false, [](bool prevBool, double value) {
            return prevBool || value < 0;
            });

        if (!hasNegativeDual)
        {
            std::cout << "Optimal solution found" << '\n';
            stop = true;
            break;
        }
        else
            // Solve the subproblem
            std::vector<Pattern> newPatterns{ PricingSubproblem(duals, env, patterns, data) };
    }

}