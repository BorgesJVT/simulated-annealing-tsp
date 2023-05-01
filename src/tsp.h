#ifndef TSP_H
#define TSP_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <random>

#include "util.h"

#define DELETE_PTR(p) if((p) != 0) { delete (p); (p) = 0; }
#define DELETE_PTRA(p) if((p) != 0) { delete[] (p); (p) = 0; }

/**
 * A city is just a point in the 2D plane
 */
typedef std::pair<float, float> City;

/**
 * This is a TSP problem instance. It defines the number of cities and the 
 * pairwise distances. 
 */
class TSPInstance {
public:
    
    /**
     * Adds a single point to the list of cities
     */
    void addCity(const std::pair<float, float> & city)
    {
        cities.push_back(city);
    }
    
    /**
     * Creates a circle of cities TSP instance of n nodes
     */
    void createCircleOfCities(int n);
    /**
     * Creates a random TSP instance of n nodes
     */
    void createRandom(int n);
    
    /**
     * Reads a TSPLIB instance from a stream
     */
    void readTSPLIB(std::istream & sin);
    
    /**
     * Sets up the distance matrix
     */
    void calcDistanceMatrix();
    
    /**
     * Calculates the length of a tour
     */
    float calcTourLength(const std::vector<int> & tour) const;
    
    /**
     * Returns the distance between cities i and j
     */
    float dist(int i, int j) const
    {
        return distances(i,j);
    }
    
    /**
     * Returns the distance between two cities
     */
    float dist(const City & c1, const City & c2) const
    {
        const float temp1 = c1.first - c2.first;
        const float temp2 = c1.second - c2.second;
        return std::sqrt((temp1*temp1+temp2*temp2));
    }
    
    /**
     * Returns the cities
     */
    const std::vector<City> & getCities() const
    {
        return cities;
    }
    
private:
    /**
     * The positions of the cities
     */
    std::vector<City> cities;
    /**
     * The distance matrix
     */
    Matrix<float> distances;
};
#endif