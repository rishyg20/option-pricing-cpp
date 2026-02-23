#include "OptionData.hpp"        // Holds option parameters (S, K, T, r, sig, b)
#include "EuropeanOption.hpp"    // Exact pricing + Greeks + finite differences
#include "Parity.hpp"            // Put-call parity calculations
#include "Mesh.hpp"              // Creates mesh vectors (e.g., 10..50 step 1)
#include "VectorPricer.hpp"      // Prices/Greeks over a vector of S values
#include "MatrixPricer.hpp"      // Prices/Greeks over a matrix of option inputs
#include "PerpetualAmericanOption.hpp"   // Perpetual American option pricing class 
#include "PerpetualVectorPricer.hpp"     // Vector pricer for Perpetual American options over S mesh
#include "PerpetualMatrixPricer.hpp"     // Matrix pricer for Perpetual American option prices


#include <string>                 // For std::string
#include <iostream>              // For console output
#include <iomanip>               // For formatting output
#include <vector>                // For std::vector
#include <cmath>                 // For std::fabs

// Helper function to print a value and compare it with expected output
static void CheckClose(const std::string& label, double got, double expected, double tol)
{
    double err = std::fabs(got - expected); // Absolute error

    std::cout << std::left << std::setw(28) << label          // Print label
              << " got=" << std::setw(12) << got              // Print computed value
              << " expected=" << std::setw(12) << expected    // Print expected value
              << " |err|=" << err                             // Print absolute error
              << (err <= tol ? "  [OK]" : "  [FAIL]")         // Pass/fail check
              << "\n";
}

int main()
{
    // Fix number of decimal places for easier comparison
    std::cout << std::fixed << std::setprecision(6);

    // ============================================================
    // A(a) Exact pricing: Test the 4 batches given 
    // For stock options, use b = r
    // ============================================================

    // Small struct to hold one batch: option inputs + expected call/put outputs
    struct Batch
    {
        OptionData d;    // Input parameters
        double Cexp;     // Expected call price
        double Pexp;     // Expected put price
    };

    // Lambda function to create stock option data (b = r)
    auto StockData = [](double S, double K, double T, double r, double sig) -> OptionData
    {
        OptionData d;    // Create empty data object
        d.S = S;         // Set spot
        d.K = K;         // Set strike
        d.T = T;         // Set maturity
        d.r = r;         // Set rate
        d.sig = sig;     // Set volatility
        d.b = r;         // For stock options, cost of carry b = r
        return d;        // Return fully populated data
    };

    // Store all 4 batches in a vector
    std::vector<Batch> batches;

    // Batch 1 expected values 
    batches.push_back({ StockData(60, 65, 0.25, 0.08, 0.30), 2.13337, 5.84628 });

    // Batch 2 expected values 
    batches.push_back({ StockData(100, 100, 1.00, 0.00, 0.20), 7.96557, 7.96557 });

    // Batch 3 expected values 
    batches.push_back({ StockData(5, 10, 1.00, 0.12, 0.50), 0.204058, 4.07326 });

    // Batch 4 expected values 
    batches.push_back({ StockData(100, 100, 30.0, 0.08, 0.30), 92.17570, 1.24750 });

    // Print heading for batch testing
    std::cout << "=== A(a) Exact European Prices (Batch 1-4) ===\n";

    // Loop through each batch and compute call/put prices
    for (std::size_t i = 0; i < batches.size(); ++i)
    {
        // Build the European option object using batch parameters
        EuropeanOption opt(batches[i].d);

        // Compute exact call and put prices
        double C = opt.CallPrice();
        double P = opt.PutPrice();

        // Print which batch we are in
        std::cout << "\nBatch " << (i + 1) << ":\n";

        // Compare computed call with expected call
        CheckClose("Call price", C, batches[i].Cexp, 1e-4);

        // Compare computed put with expected put
        CheckClose("Put  price", P, batches[i].Pexp, 1e-4);
    }

    // ============================================================
    // A(b) Put-Call parity: compute put from call, call from put
    // and also check parity condition
    // ============================================================

    std::cout << "\n=== A(b) Put-Call Parity (compute + check) ===\n";

    for (std::size_t i = 0; i < batches.size(); ++i)
    {
        // Create option object for this batch
        EuropeanOption opt(batches[i].d);

        // Compute call and put prices directly
        double C = opt.CallPrice();
        double P = opt.PutPrice();

        // Compute put from call using parity
        double P_from_C = Parity::PutFromCall(C, batches[i].d);

        // Compute call from put using parity
        double C_from_P = Parity::CallFromPut(P, batches[i].d);

        // Check whether parity holds
        bool ok = Parity::Check(C, P, batches[i].d, 1e-6);

        std::cout << "\nBatch " << (i + 1) << ":\n";

        // Compare parity-derived put with exact put
        CheckClose("Put from Call (parity)", P_from_C, P, 1e-6);

        // Compare parity-derived call with exact call
        CheckClose("Call from Put (parity)", C_from_P, C, 1e-6);

        // Print final parity pass/fail
        std::cout << "Parity check: " << (ok ? "PASS" : "FAIL") << "\n";
    }

    // ============================================================
    // A(c) Vector pricing over a mesh of S
    // The PDF suggests S=10,11,...,50 and storing outputs in vector
    // ============================================================

    std::cout << "\n=== A(c) Vector pricing over S mesh (Batch 1 base params) ===\n";

    // Create a mesh of S values from 10 to 50 with step size 1
    std::vector<double> meshS = Mesh::Create(10.0, 50.0, 1.0);

    // Use Batch 1 parameters as base (we will only vary S)
    OptionData base = batches[0].d;

    // Compute call prices over the mesh
    std::vector<double> callVec = VectorPricer::CallPricesOverS(base, meshS);

    // Compute put prices over the mesh
    std::vector<double> putVec = VectorPricer::PutPricesOverS(base, meshS);

    // Print first few values to prove it worked
    std::cout << "First 5 call prices: ";
    for (int i = 0; i < 5; ++i)
        std::cout << callVec[i] << " ";
    std::cout << "\n";

    std::cout << "First 5 put prices : ";
    for (int i = 0; i < 5; ++i)
        std::cout << putVec[i] << " ";
    std::cout << "\n";

    // ============================================================
    // A(d) Matrix pricing
    // The PDF asks for a matrix pricer where you can vary any parameter
    // ============================================================

    std::cout << "\n=== A(d) Matrix pricing (vary S and sig) ===\n";

    // Define a small grid of S values (rows)
    std::vector<double> Sgrid = { 40, 50, 60, 70, 80 };

    // Define a small grid of volatilities (columns)
    std::vector<double> sigGrid = { 0.10, 0.20, 0.30, 0.40 };

    // Create a matrix (vector of vectors) of OptionData
    std::vector<std::vector<OptionData>> grid;

    for (double S : Sgrid)
    {
        std::vector<OptionData> row;   // One row for this S value

        for (double sig : sigGrid)
        {
            OptionData d = base;  // Start from base parameters
            d.S = S;              // Set row S
            d.sig = sig;          // Set column volatility
            row.push_back(d);     // Add this parameter set to the row
        }

        grid.push_back(row);      // Add the completed row to the grid
    }

    // Compute call prices for the full matrix
    std::vector<std::vector<double>> callMat = MatrixPricer::CallPrices(grid);

    // Print one entry as an example
    std::cout << "Example CallMat[0][0] (S=40, sig=0.10): " << callMat[0][0] << "\n";

    // ============================================================
    // Greeks (a): Futures option dataset from PDF (b = 0)
    // Compute exact call delta and put delta and compare to expected
    // ============================================================

    std::cout << "\n=== Greeks (a) Futures option deltas (exact) ===\n";

    OptionData fut;      // Create a new OptionData for futures case
    fut.K = 100;         // Strike
    fut.S = 105;         // Spot
    fut.T = 0.5;         // Time to expiry
    fut.r = 0.1;         // Rate
    fut.b = 0.0;         // Futures option: cost of carry b = 0
    fut.sig = 0.36;      // Volatility

    // Create option object for futures case
    EuropeanOption futOpt(fut);

    // Compute exact deltas
    double dCall = futOpt.CallDelta();
    double dPut  = futOpt.PutDelta();

    // Compute exact gamma
    double gExact = futOpt.Gamma();

    // Compare deltas to expected values in PDF
    CheckClose("Call Delta", dCall, 0.5946, 1e-4);
    CheckClose("Put  Delta", dPut, -0.3566, 1e-4);

    // Print gamma value 
    std::cout << "Gamma (exact) = " << gExact << "\n";

    // ============================================================
    // Greeks (b): Delta over S mesh using futures parameters
    // ============================================================

    std::cout << "\n=== Greeks (b) Call Delta over S mesh (futures params) ===\n";

    // Compute call delta over mesh of S using futures parameters
    std::vector<double> deltaVec = VectorPricer::CallDeltasOverS(fut, meshS);

    // Print first few values
    std::cout << "First 5 call deltas: ";
    for (int i = 0; i < 5; ++i)
        std::cout << deltaVec[i] << " ";
    std::cout << "\n";

    // ============================================================
    // Greeks (c): Matrix Greeks (Delta and Gamma)
    // Example: vary S and sig again using futures base parameters
    // ============================================================

    std::cout << "\n=== Greeks (c) Matrix Delta/Gamma (vary S and sig) ===\n";

    std::vector<std::vector<OptionData>> gGrid;

    for (double S : Sgrid)
    {
        std::vector<OptionData> row;

        for (double sig : sigGrid)
        {
            OptionData d = fut;   // Copy futures base params
            d.S = S;              // Set row S
            d.sig = sig;          // Set column sig
            row.push_back(d);     // Add to row
        }

        gGrid.push_back(row);
    }

    // Compute matrix of call deltas
    std::vector<std::vector<double>> dMat = MatrixPricer::CallDeltas(gGrid);

    // Compute matrix of gammas
    std::vector<std::vector<double>> gMat = MatrixPricer::Gammas(gGrid);

    // Print one example entry from each matrix
    std::cout << "Example DeltaMat[0][0] = " << dMat[0][0] << "\n";
    std::cout << "Example GammaMat[0][0] = " << gMat[0][0] << "\n";

    // ============================================================
    // Greeks (d): Divided differences / finite differences
    // Compare FD delta/gamma vs exact for multiple step sizes h
    // ============================================================

    std::cout << "\n=== Greeks (d) Finite differences vs exact (futures call) ===\n";

    // Store exact values for comparison
    double exactDelta = futOpt.CallDelta();
    double exactGamma = futOpt.Gamma();

    // Try multiple step sizes to see accuracy changes
    for (double h : { 1.0, 0.5, 0.1, 0.01 })
    {
        // Compute finite-difference delta and gamma
        double dFD = futOpt.CallDeltaFD(h);
        double gFD = futOpt.CallGammaFD(h);

        // Print FD values and errors
        std::cout << "h=" << std::setw(6) << h
                  << "  DeltaFD=" << std::setw(12) << dFD
                  << "  |err|=" << std::fabs(dFD - exactDelta)
                  << "  GammaFD=" << std::setw(12) << gFD
                  << "  |err|=" << std::fabs(gFD - exactGamma)
                  << "\n";
    }

        // ============================================================
    // B(b) Test Perpetual American option with given parameters
    // K=100, sig=0.1, r=0.1, b=0.02, S=110
    // Check: C=18.5035, P=3.03106
    // ============================================================

    std::cout << "\n=== B(b) Perpetual American option test ===\n";

    OptionData per;
    per.K = 100.0;       // Strike
    per.sig = 0.10;      // Volatility
    per.r = 0.10;        // Rate
    per.b = 0.02;        // Cost of carry
    per.S = 110.0;       // Spot
    per.T = 1.0;         // Not used, but we can set it to any positive value

    PerpetualAmericanOption perOpt(per);

    double Cper = perOpt.CallPrice(); // Compute perpetual call
    double Pper = perOpt.PutPrice();  // Compute perpetual put

    CheckClose("Perpetual Call", Cper, 18.5035, 1e-4);
    CheckClose("Perpetual Put ", Pper,  3.03106, 1e-4);

    // ============================================================
    // B(c) Vector pricing over S mesh (S = 10..50 step 1)
    // ============================================================

    std::cout << "\n=== B(c) Perpetual vector pricing over S mesh ===\n";

    std::vector<double> meshS2 = Mesh::Create(10.0, 50.0, 1.0);

    std::vector<double> perCallVec = PerpetualVectorPricer::CallPricesOverS(per, meshS2);
    std::vector<double> perPutVec  = PerpetualVectorPricer::PutPricesOverS(per, meshS2);

    std::cout << "First 5 perpetual call prices: ";
    for (int i = 0; i < 5; ++i)
        std::cout << perCallVec[i] << " ";
    std::cout << "\n";

    std::cout << "First 5 perpetual put prices : ";
    for (int i = 0; i < 5; ++i)
        std::cout << perPutVec[i] << " ";
    std::cout << "\n";

    // ============================================================
    // B(d) Matrix pricing (example: vary S and sig)
    // ============================================================

    std::cout << "\n=== B(d) Perpetual matrix pricing (vary S and sig) ===\n";

    std::vector<double> Sgrid2   = { 80, 90, 100, 110, 120 };
    std::vector<double> siggrid2 = { 0.05, 0.10, 0.20 };

    std::vector<std::vector<OptionData>> perGrid;

    for (double S : Sgrid2)
    {
        std::vector<OptionData> row;

        for (double sig : siggrid2)
        {
            OptionData d = per;  // Copy base
            d.S = S;             // Set S
            d.sig = sig;         // Set sig
            row.push_back(d);    // Store in row
        }

        perGrid.push_back(row);  // Store row in matrix
    }

    // Large perpetual American put values for small S are expected 
    // due to immediate optimal exercise in the absence of maturity.

    std::vector<std::vector<double>> perCallMat = PerpetualMatrixPricer::CallPrices(perGrid);

    std::cout << "Example Perpetual CallMat[0][0] (S=80, sig=0.05): " << perCallMat[0][0] << "\n";


    // End of program
    return 0;
}
