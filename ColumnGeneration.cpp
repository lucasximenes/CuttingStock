#include "ColumnGeneration.h"

std::vector<Pattern> PricingSubproblem(const std::vector<double>& duals, const IloEnv& env, const std::vector<Pattern>& patterns, const CSPData& data)
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

ColumnGeneration::ColumnGeneration(const CSPData& data)
{
    IloEnv env{};
    IloModel master{ env };


    // Create initial patterns
    std::vector<Pattern> patterns{ data.GenerateInitialPatterns() };
    /*for (auto& pattern : patterns)
    {
        std::cout << "Pattern: ";
        for (auto& p : pattern)
        {
            std::cout << p << ' ';
        }
        std::cout << '\n';
    }*/

    // Create the initial variables
    IloNumVarArray lambda{ env, patterns.size(), 0, IloInfinity };

    // Create the objective function
    IloExpr obj{ env };
    for (int i = 0; i < patterns.size(); i++)
    {
        obj += lambda[i];
    }
    master.add(IloMinimize(env, obj));
    obj.end();

    // Create the demand constraints
    IloRangeArray cons{ env, data.orders.size() };
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

    // Start column generation loop
    bool stop{ false };
    while (!stop)
    {
        // Solve the master problem
        IloCplex cplex{ master };
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