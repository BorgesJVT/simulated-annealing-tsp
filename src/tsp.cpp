#include "tsp.h"

////////////////////////////////////////////////////////////////////////////////
/// TSPInstance
////////////////////////////////////////////////////////////////////////////////

void TSPInstance::createCircleOfCities(int n)
{
    float dTheta = 2*M_PI/n;
    // We generate cities on a 1000x1000 pixel plane
    for (int i = 0; i < n; i++)
    {
        // Create a city in a circle of radius 100
        float x = 100.0*cos(dTheta*i) + 100.0;
        float y = 100.0*sin(dTheta*i) + 100.0;
        City city = std::make_pair(x, y);
        // Add the city
        addCity(city);
    }
}

void TSPInstance::createRandom(int n)
{
    // We generate cities on a 1000x1000 pixel plane
    std::mt19937 generator({std::random_device{}()});
    std::uniform_real_distribution<float> distribution(0.0f,999.0f);

    for (int i = 0; i < n; i++)
    {
        // Create a random city
        City city = std::make_pair(distribution(generator), distribution(generator));
        // Add the city
        addCity(city);
    }
}

void TSPInstance::readTSPLIB(std::istream & sin)
{
    // Wait for the NODE_COORD_SECTION token
    const std::string startToken = "NODE_COORD_SECTION";
    const std::string endToken = "EOF";

    std::string parser;
    do {
        sin >> parser;
    } while (parser != startToken);

    // Parse the cities
    while (true)
    {
        City city;

        // The first element is the ID of the city or the EOF tag
        sin >> parser;
        if (parser == endToken)
        {
            // We are done
            break;
        }

        // Now come the two coordinates
        sin >> city.first;
        sin >> city.second;
        addCity(city);
    }
}

void TSPInstance::calcDistanceMatrix()
{
    // Get the number of cities
    int n = static_cast<int>(cities.size());

    // Allocate the new one
    distances = Matrix<float>(n,n);

    for (int i = 0; i < n; i++)
    {
        for (int j = i; j < n; j++)
        {
            // The distance matrix is symmetric
            distances(i,j) = dist(cities[i], cities[j]);
            distances(j,i) = dist(cities[i], cities[j]);
        }
    }
}

float TSPInstance::calcTourLength(const std::vector<int> & tour) const
{
    assert(tour.size() == cities.size());
    
    float result = 0.0f;
    // Calculate the length of the chain
    for (size_t i = 0; i < tour.size() - 1; i++)
    {
        result += distances(tour[i], tour[i+1]);
    }
    // Close the loop
    result += distances(tour[tour.size() - 1], tour[0]);
    
    return result;
}