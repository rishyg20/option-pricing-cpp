#ifndef MATRIXPRICER_HPP
#define MATRIXPRICER_HPP

#include "EuropeanOption.hpp"
#include <vector>

// Class to price options on a matrix of parameters
class MatrixPricer
{
public:
    // Matrix of call prices
    static std::vector<std::vector<double>>
    CallPrices(const std::vector<std::vector<OptionData>>& grid)
    {
        return Apply(grid, 0);
    }

    // Matrix of put prices
    static std::vector<std::vector<double>>
    PutPrices(const std::vector<std::vector<OptionData>>& grid)
    {
        return Apply(grid, 1);
    }

    // Matrix of call deltas
    static std::vector<std::vector<double>>
    CallDeltas(const std::vector<std::vector<OptionData>>& grid)
    {
        return Apply(grid, 2);
    }

    // Matrix of gammas
    static std::vector<std::vector<double>>
    Gammas(const std::vector<std::vector<OptionData>>& grid)
    {
        return Apply(grid, 3);
    }

private:
    // Internal helper to avoid code duplication
    static std::vector<std::vector<double>>
    Apply(const std::vector<std::vector<OptionData>>& grid, int mode)
    {
        std::vector<std::vector<double>> result;

        for (const auto& row : grid)
        {
            std::vector<double> values;

            for (const auto& d : row)
            {
                EuropeanOption opt(d);

                if (mode == 0)      values.push_back(opt.CallPrice());
                else if (mode == 1) values.push_back(opt.PutPrice());
                else if (mode == 2) values.push_back(opt.CallDelta());
                else                values.push_back(opt.Gamma());
            }

            result.push_back(values);
        }

        return result;
    }
};

#endif
