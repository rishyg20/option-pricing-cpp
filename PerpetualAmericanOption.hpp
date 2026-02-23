#ifndef PERPETUALAMERICANOPTION_HPP
#define PERPETUALAMERICANOPTION_HPP

#include "OptionData.hpp"   // Reuse same data container (we will ignore T)
#include <cmath>            // For sqrt, pow
#include <stdexcept>        // For exceptions

// Class to price Perpetual American options using the exact closed-form formulas
class PerpetualAmericanOption
{
private:
    OptionData m_data;      // Store S, K, r, sig, b (T is not used for perpetual)

    // y1 parameter used in call formula
    double y1() const;

    // y2 parameter used in put formula
    double y2() const;

public:
    // Constructor stores input data and validates basic parameters
    explicit PerpetualAmericanOption(const OptionData& data);

    // Exact Perpetual American call price
    double CallPrice() const;

    // Exact Perpetual American put price
    double PutPrice() const;

    // Return stored data (useful for debugging/testing)
    const OptionData& Data() const;
};

#endif
