#include "optimizer.h"
#include "runtimegui.h"

////////////////////////////////////////////////////////////////////////////////
/// Optimizer
////////////////////////////////////////////////////////////////////////////////

// Function to generate all possible permutations of cities
std::vector<std::vector<int>> generatePermutations(std::vector<int> cities)
{
    std::vector<std::vector<int>> permutations;
    do {
        permutations.push_back(cities);
    } while (std::next_permutation(cities.begin() + 1, cities.end()));
    return permutations;
}

// Brute force
void Optimizer::optimize(const TSPInstance& instance, std::vector<int> &
result)
{
    // Start timer
    auto lastUpdate = std::chrono::system_clock::now();

    // Get the number of cities
    int numCities = static_cast<int>(instance.getCities().size());

    // Generate all possible permutations of cities
    std::vector<int> cities(numCities);
    for (int i = 0; i < numCities; ++i) {
        cities[i] = i;
    }
    std::vector<std::vector<int>> allTours = generatePermutations(cities);

    // Find the permutation with the minimum tour length
    float minTourLength = std::numeric_limits<float>::max();
    std::vector<int> optimalTour;

    for (const auto& tour : allTours) {
        float tourLength = instance.calcTourLength(tour);
        if (tourLength < minTourLength) {
            minTourLength = tourLength;
            optimalTour = tour;
        }
        // Notify observers
        state_ = tour;
        energy_ = tourLength;
        bestEnergy_ = minTourLength;
        bestState_ = optimalTour;
        observers_[0]->notify(instance, *this);
    }

    // compute time difference to start timer
    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();
    timeExecution_ = timeSinceLastUpdate;
    std::cout << "Elapsed time for optimization: " << timeSinceLastUpdate << "ms"
            << std::endl;
    std::cout << "Optimal tour length: " << minTourLength << std::endl;
}
