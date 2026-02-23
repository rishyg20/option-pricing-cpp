#include "PerpetualAmericanOption.hpp"

// Constructor: store data and validate required fields
PerpetualAmericanOption::PerpetualAmericanOption(const OptionData& data)
    : m_data(data)
{
    // For perpetual options, we still need S, K, sig
    if (m_data.S <= 0.0)
        throw std::invalid_argument("S must be > 0");

    if (m_data.K <= 0.0)
        throw std::invalid_argument("K must be > 0");

    if (m_data.sig <= 0.0)
        throw std::invalid_argument("sig must be > 0");

    // r and b can be any real values in general; no strict restriction here
}

// Return the stored input data
const OptionData& PerpetualAmericanOption::Data() const
{
    return m_data;
}

// Compute y1 used in call formula
double PerpetualAmericanOption::y1() const
{
    // Precompute sigma^2
    double sig2 = m_data.sig * m_data.sig;

    // Compute term (b/sig^2 - 1/2)
    double term = (m_data.b / sig2) - 0.5;

    // Compute inside of the square root: term^2 + 2r/sig^2
    double disc = term * term + (2.0 * m_data.r / sig2);

    // Safety check: must be non-negative to take sqrt
    if (disc < 0.0)
        throw std::runtime_error("Invalid parameters: sqrt discriminant < 0 in y1()");

    // Compute y1 = 1/2 - b/sig^2 + sqrt(...)
    return 0.5 - (m_data.b / sig2) + std::sqrt(disc);
}

// Compute y2 used in put formula
double PerpetualAmericanOption::y2() const
{
    double sig2 = m_data.sig * m_data.sig;
    double term = (m_data.b / sig2) - 0.5;
    double disc = term * term + (2.0 * m_data.r / sig2);

    if (disc < 0.0)
        throw std::runtime_error("Invalid parameters: sqrt discriminant < 0 in y2()");

    // Compute y2 = 1/2 - b/sig^2 - sqrt(...)
    return 0.5 - (m_data.b / sig2) - std::sqrt(disc);
}

// Exact perpetual American call price
double PerpetualAmericanOption::CallPrice() const
{
    double Y1 = y1(); // Compute y1

    // For the formula, denominator (y1 - 1) must not be zero
    if (std::fabs(Y1 - 1.0) < 1e-12)
        throw std::runtime_error("Invalid parameters: y1 is too close to 1 in CallPrice()");

    // Compute A = (y1 - 1)/y1 * (S/K)
    double A = ((Y1 - 1.0) / Y1) * (m_data.S / m_data.K);

    // A must be positive because we raise it to power y1
    if (A <= 0.0)
        throw std::runtime_error("Invalid parameters: base of power <= 0 in CallPrice()");

    // Call formula: C = (K/(y1-1)) * A^{y1}
    return (m_data.K / (Y1 - 1.0)) * std::pow(A, Y1);
}

// Exact perpetual American put price
double PerpetualAmericanOption::PutPrice() const
{
    double Y2 = y2(); // Compute y2

    // For the formula, denominator (1 - y2) must not be zero
    if (std::fabs(1.0 - Y2) < 1e-12)
        throw std::runtime_error("Invalid parameters: y2 is too close to 1 in PutPrice()");

    // Compute B = (y2 - 1)/y2 * (S/K)
    double B = ((Y2 - 1.0) / Y2) * (m_data.S / m_data.K);

    // B must be positive because we raise it to power y2
    if (B <= 0.0)
        throw std::runtime_error("Invalid parameters: base of power <= 0 in PutPrice()");

    // Put formula: P = (K/(1-y2)) * B^{y2}
    return (m_data.K / (1.0 - Y2)) * std::pow(B, Y2);
}
