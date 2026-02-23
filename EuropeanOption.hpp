#ifndef EUROPEANOPTION_HPP
#define EUROPEANOPTION_HPP

#include "OptionData.hpp"

class EuropeanOption
{
private:
    OptionData m_data; // Store all option parameters

// Helper functions for Black–Scholes formula
    double d1() const;
    double d2() const;

public:
    // Constructor takes option data as input
    explicit EuropeanOption(const OptionData& data); 

    // Exact prices (A.a)
    double CallPrice() const; // Price of a European call
    double PutPrice() const; // Price of a European put

    // Exact greeks (Greeks a-c)
    double CallDelta() const; // Delta of a call option
    double PutDelta() const; // Delta of a put option
    double Gamma() const; // same for call/put

    // Divided differences (Greeks d)
    double CallDeltaFD(double h) const; 
    double PutDeltaFD(double h) const;
    double CallGammaFD(double h) const;
    double PutGammaFD(double h) const;

    //Access stored option data
    const OptionData& Data() const;
};

#endif
