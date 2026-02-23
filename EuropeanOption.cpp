#include "EuropeanOption.hpp"
#include "Normal.hpp"

#include <cmath>       // For log, sqrt, exp
#include <stdexcept>   // For exceptions

// Constructor: store option data and validate it
EuropeanOption::EuropeanOption(const OptionData& data)
    : m_data(data)
{
    m_data.Validate(); // Ensure parameters are valid
}

// Return the stored option data
const OptionData& EuropeanOption::Data() const
{
    return m_data;
}

// Compute d1 used in Black–Scholes formula
double EuropeanOption::d1() const
{
    double sigSqrtT = m_data.sig * std::sqrt(m_data.T); // sigma√T

    return (std::log(m_data.S / m_data.K)               // ln(S/K)
          + (m_data.b + 0.5 * m_data.sig * m_data.sig)  // (b + sigma²/2)
            * m_data.T)
          / sigSqrtT;                                   // Divide by sigma√T
}

// Compute d2 = d1 − σ√T
double EuropeanOption::d2() const
{
    return d1() - m_data.sig * std::sqrt(m_data.T);
}

// Exact European call price
double EuropeanOption::CallPrice() const
{
    double D1 = d1();  // Compute d1
    double D2 = d2();  // Compute d2

    double discCarry = std::exp((m_data.b - m_data.r) * m_data.T); // e^{(b-r)T}
    double discR     = std::exp(-m_data.r * m_data.T);             // e^{-rT}

    return m_data.S * discCarry * N(D1)     // First term
         - m_data.K * discR * N(D2);         // Second term
}

// Exact European put price
double EuropeanOption::PutPrice() const
{
    double D1 = d1();  // Compute d1
    double D2 = d2();  // Compute d2

    double discCarry = std::exp((m_data.b - m_data.r) * m_data.T);
    double discR     = std::exp(-m_data.r * m_data.T);

    return m_data.K * discR * N(-D2)        // First term
         - m_data.S * discCarry * N(-D1);   // Second term
}

// Exact delta of a call option
double EuropeanOption::CallDelta() const
{
    double discCarry = std::exp((m_data.b - m_data.r) * m_data.T);
    return discCarry * N(d1());
}

// Exact delta of a put option
double EuropeanOption::PutDelta() const
{
    double discCarry = std::exp((m_data.b - m_data.r) * m_data.T);
    return discCarry * (N(d1()) - 1.0);
}

// Exact gamma (same for call and put)
double EuropeanOption::Gamma() const
{
    double discCarry = std::exp((m_data.b - m_data.r) * m_data.T);

    return discCarry * n(d1())                 // n(d1)
         / (m_data.S * m_data.sig * std::sqrt(m_data.T)); // Denominator
}

// Central finite difference for first derivative
static double CentralFirst(double f_up, double f_dn, double h)
{
    return (f_up - f_dn) / (2.0 * h);
}

// Central finite difference for second derivative
static double CentralSecond(double f_up, double f_mid, double f_dn, double h)
{
    return (f_up - 2.0 * f_mid + f_dn) / (h * h);
}

// Finite-difference call delta
double EuropeanOption::CallDeltaFD(double h) const
{
    if (h <= 0.0)
        throw std::invalid_argument("h must be > 0");

    OptionData up = m_data;  // Copy original data
    OptionData dn = m_data;

    up.S += h;               // Increase spot
    dn.S -= h;               // Decrease spot

    if (dn.S <= 0.0)
        throw std::invalid_argument("S-h must be > 0");

    EuropeanOption optUp(up);
    EuropeanOption optDn(dn);

    return CentralFirst(optUp.CallPrice(), optDn.CallPrice(), h);
}

// Finite-difference put delta
double EuropeanOption::PutDeltaFD(double h) const
{
    if (h <= 0.0)
        throw std::invalid_argument("h must be > 0");

    OptionData up = m_data;  // Copy original data
    OptionData dn = m_data;

    up.S += h;               // Increase spot
    dn.S -= h;               // Decrease spot

    if (dn.S <= 0.0)
        throw std::invalid_argument("S-h must be > 0");

    EuropeanOption optUp(up);
    EuropeanOption optDn(dn);

    return CentralFirst(optUp.PutPrice(), optDn.PutPrice(), h);
}

// Finite-difference call gamma
double EuropeanOption::CallGammaFD(double h) const
{
    if (h <= 0.0)
        throw std::invalid_argument("h must be > 0");

    OptionData up = m_data;  // Copy original data
    OptionData dn = m_data;

    up.S += h;               // Increase spot
    dn.S -= h;               // Decrease spot

    if (dn.S <= 0.0)
        throw std::invalid_argument("S-h must be > 0");

    EuropeanOption optUp(up);
    EuropeanOption optMid(m_data);
    EuropeanOption optDn(dn);

    return CentralSecond(optUp.CallPrice(), optMid.CallPrice(), optDn.CallPrice(), h);
}

// Finite-difference put gamma
double EuropeanOption::PutGammaFD(double h) const
{
    if (h <= 0.0)
        throw std::invalid_argument("h must be > 0");

    OptionData up = m_data;  // Copy original data
    OptionData dn = m_data;

    up.S += h;               // Increase spot
    dn.S -= h;               // Decrease spot

    if (dn.S <= 0.0)
        throw std::invalid_argument("S-h must be > 0");

    EuropeanOption optUp(up);
    EuropeanOption optMid(m_data);
    EuropeanOption optDn(dn);

    return CentralSecond(optUp.PutPrice(), optMid.PutPrice(), optDn.PutPrice(), h);
}

