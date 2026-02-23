#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <stdexcept>


class Mesh
{
public:
    // Create a vector from start to end with step size h
    static std::vector<double> Create(double start, double end, double h)
    {
        if (h <= 0.0)
            throw std::invalid_argument("Step size must be positive");

        if (end < start)
            throw std::invalid_argument("End must be >= start");

        std::vector<double> mesh;

        for (double x = start; x <= end + 0.5 * h; x += h)
            mesh.push_back(x); // Add next mesh point

        return mesh;
    }
};

#endif
