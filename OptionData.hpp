#ifndef OPTIONDATA_HPP
#define OPTIONDATA_HPP

#include <stdexcept> //For exceptio handling

struct OptionData
{
    double S;    // Spot price of the underlying
    double K;    // Strike price
    double T;    // Time to maturity 
    double r;    // Risk-free interest rate
    double sig;  // Volatility
    double b;    // Cost of carry

    OptionData() : S(0.0), K(0.0), T(0.0), r(0.0), sig(0.0), b(0.0) {} //default constructor 

//Checking if inputs are valid or not    
    void Validate() const
    {
        // spot price, strike price, time to maturity and volatility should be positive
        if (S <= 0.0)   throw std::invalid_argument("S must be > 0"); 
        if (K <= 0.0)   throw std::invalid_argument("K must be > 0");
        if (T <= 0.0)   throw std::invalid_argument("T must be > 0");
        if (sig <= 0.0) throw std::invalid_argument("sig must be > 0");
        // r and b can be negative; hence not restricted here.
    }
};

#endif
