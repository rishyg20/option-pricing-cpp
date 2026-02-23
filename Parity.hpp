#ifndef PARITY_HPP
#define PARITY_HPP

#include "OptionData.hpp"
#include <cmath>

// Class to handle put-call parity calculations
class Parity
{
public:
    // Compute put price using call price and parity formula
    static double PutFromCall(double callPrice, const OptionData& d)
    {
        d.Validate(); // Ensure parameters are valid

        return callPrice
             + d.K * std::exp(-d.r * d.T)           // Discounted strike
             - d.S * std::exp((d.b - d.r) * d.T);   // Discounted spot
    }

    // Compute call price using put price and parity formula
    static double CallFromPut(double putPrice, const OptionData& d)
    {
        d.Validate();

        return putPrice
             + d.S * std::exp((d.b - d.r) * d.T)
             - d.K * std::exp(-d.r * d.T);
    }

    // Check whether put-call parity holds within a tolerance
    static bool Check(double callPrice, double putPrice,
                      const OptionData& d, double tol)
    {
        d.Validate();

        double lhs = callPrice + d.K * std::exp(-d.r * d.T); // Left-hand side
        double rhs = putPrice  + d.S * std::exp((d.b - d.r) * d.T); // Right-hand side

        return std::fabs(lhs - rhs) <= tol;
    }
};

#endif
