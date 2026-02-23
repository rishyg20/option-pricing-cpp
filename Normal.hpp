#ifndef NORMAL_HPP
#define NORMAL_HPP

#include <boost/math/distributions/normal.hpp> // Boost normal distribution

inline double N(double x)   // CDF N(x)
{
    static const boost::math::normal norm;
    return boost::math::cdf(norm, x); // return CDF value
}

inline double n(double x)   // PDF n(x)
{
    static const boost::math::normal norm;
    return boost::math::pdf(norm, x); // return PDF value
}

#endif
