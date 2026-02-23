#ifndef VECTORPRICER_HPP
#define VECTORPRICER_HPP

#include "EuropeanOption.hpp"
#include <vector>

// Class to price options over a vector of underlying prices
class VectorPricer
{
public:
    // Call prices over a mesh of S
    static std::vector<double> CallPricesOverS(const OptionData& base,
                                               const std::vector<double>& meshS)
    {
        std::vector<double> prices;

        for (double S : meshS)
        {
            OptionData d = base; // Copy base parameters
            d.S = S;             // Update spot
            prices.push_back(EuropeanOption(d).CallPrice());
        }

        return prices;
    }

    // Put prices over a mesh of S
    static std::vector<double> PutPricesOverS(const OptionData& base,
                                              const std::vector<double>& meshS)
    {
        std::vector<double> prices;

        for (double S : meshS)
        {
            OptionData d = base;
            d.S = S;
            prices.push_back(EuropeanOption(d).PutPrice());
        }

        return prices;
    }

    // Call deltas over a mesh of S
    static std::vector<double> CallDeltasOverS(const OptionData& base,
                                               const std::vector<double>& meshS)
    {
        std::vector<double> deltas;

        for (double S : meshS)
        {
            OptionData d = base;
            d.S = S;
            deltas.push_back(EuropeanOption(d).CallDelta());
        }

        return deltas;
    }
};

#endif
