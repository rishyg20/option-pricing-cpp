# Option Pricing Library (C++)

Black-Scholes pricing library for European and Perpetual American options with Greeks and batch pricing capabilities.

![C++](https://img.shields.io/badge/C++-14+-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## Features

- **European Options:** Black-Scholes exact pricing (calls/puts)
- **Perpetual American Options:** Closed-form solutions
- **Greeks:** Delta, Gamma (exact + finite difference)
- **Put-Call Parity:** Verification and conversion
- **Batch Pricing:** Vector/matrix pricers for sensitivity analysis

## Quick Example
```cpp
#include "EuropeanOption.hpp"

OptionData data;
data.S = 100.0;   // Spot
data.K = 100.0;   // Strike
data.T = 1.0;     // Maturity
data.r = 0.05;    // Rate
data.sig = 0.20;  // Volatility
data.b = 0.05;    // Cost of carry

EuropeanOption opt(data);
double call = opt.CallPrice();   // 10.4506
double delta = opt.CallDelta();  // 0.6368
```

## Project Structure

| File | Description |
|------|-------------|
| `OptionData.hpp` | Container struct for option parameters (S, K, T, r, σ, b) with validation |
| `Normal.hpp` | Wrapper for Boost normal distribution CDF and PDF |
| `EuropeanOption.hpp/cpp` | Black-Scholes pricing for European calls/puts + exact Greeks (Delta, Gamma) + finite difference Greeks |
| `PerpetualAmericanOption.hpp/cpp` | Closed-form pricing for perpetual American calls/puts |
| `Parity.hpp` | Put-call parity: compute put from call, call from put, verify parity holds |
| `Mesh.hpp` | Utility to generate parameter meshes (e.g., S = 80, 81, ..., 120) |
| `VectorPricer.hpp` | Price European options over a vector of spot prices |
| `MatrixPricer.hpp` | Price European options over a 2D grid of parameters (e.g., S × σ) |
| `PerpetualVectorPricer.hpp` | Price perpetual American options over a vector of spot prices |
| `PerpetualMatrixPricer.hpp` | Price perpetual American options over a 2D parameter grid |
| `main.cpp` | Test harness validating all functionality against known values |

## Validation

Tested against standard option pricing benchmarks:

| Test Case | Call | Put | Status |
|-----------|------|-----|--------|
| S=60, K=65, T=0.25, r=0.08, σ=0.30 | 2.1334 | 5.8463 | ✓ |
| S=100, K=100, T=1.0, r=0.0, σ=0.20 | 7.9656 | 7.9656 | ✓ |
| Perpetual: S=110, K=100, r=0.10, σ=0.10 | 18.5035 | 3.0311 | ✓ |

## Build
```bash
g++ -std=c++14 -I/path/to/boost main.cpp EuropeanOption.cpp PerpetualAmericanOption.cpp -o pricer
./pricer
```

**Requirements:** C++14, Boost library

## Author

Rishabh Gupta
