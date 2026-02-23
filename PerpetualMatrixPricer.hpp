#ifndef PERPETUALMATRIXPRICER_HPP
#define PERPETUALMATRIXPRICER_HPP

#include "PerpetualAmericanOption.hpp"
#include <vector>

// Class to price Perpetual American options on a matrix of OptionData
class PerpetualMatrixPricer
{
public:
    // Matrix of call prices
    static std::vector<std::vector<double>>
    CallPrices(const std::vector<std::vector<OptionData>>& grid)
    {
        return Apply(grid, 0); // mode 0 => call
    }

    // Matrix of put prices
    static std::vector<std::vector<double>>
    PutPrices(const std::vector<std::vector<OptionData>>& grid)
    {
        return Apply(grid, 1); // mode 1 => put
    }

private:
    // Internal helper to avoid duplicating loops
    static std::vector<std::vector<double>>
    Apply(const std::vector<std::vector<OptionData>>& grid, int mode)
    {
        std::vector<std::vector<double>> out;     // Output matrix
        out.reserve(grid.size());                  // Reserve number of rows

        for (const auto& row : grid)               // Loop over each row
        {
            std::vector<double> prow;              // Output row
            prow.reserve(row.size());              // Reserve number of columns

            for (const auto& d : row)              // Loop over each cell (OptionData)
            {
                PerpetualAmericanOption opt(d);    // Build perpetual option

                if (mode == 0)
                    prow.push_back(opt.CallPrice()); // Store call
                else
                    prow.push_back(opt.PutPrice());  // Store put
            }

            out.push_back(prow);                   // Add row to matrix
        }

        return out;                                // Return matrix
    }
};

#endif
