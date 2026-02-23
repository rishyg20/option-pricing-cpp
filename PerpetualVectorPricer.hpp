#ifndef PERPETUALVECTORPRICER_HPP
#define PERPETUALVECTORPRICER_HPP

#include "PerpetualAmericanOption.hpp"
#include <vector>

// Class to price Perpetual American options over a vector of S values
class PerpetualVectorPricer
{
public:
    // Call prices over S mesh
    static std::vector<double> CallPricesOverS(const OptionData& base,
                                               const std::vector<double>& meshS)
    {
        std::vector<double> out;           // Output vector
        out.reserve(meshS.size());         // Reserve memory for efficiency

        for (double S : meshS)             // Loop over each S
        {
            OptionData d = base;           // Copy base data
            d.S = S;                       // Replace spot
            out.push_back(PerpetualAmericanOption(d).CallPrice()); // Store call price
        }

        return out;                        // Return vector of prices
    }

    // Put prices over S mesh
    static std::vector<double> PutPricesOverS(const OptionData& base,
                                              const std::vector<double>& meshS)
    {
        std::vector<double> out;
        out.reserve(meshS.size());

        for (double S : meshS)
        {
            OptionData d = base;
            d.S = S;
            out.push_back(PerpetualAmericanOption(d).PutPrice()); // Store put price
        }

        return out;
    }
};

#endif
