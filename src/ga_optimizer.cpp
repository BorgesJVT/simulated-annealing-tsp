#include "optimizer.h"

////////////////////////////////////////////////////////////////////////////////
/// Simple Genetic Algorithm Optimizer
////////////////////////////////////////////////////////////////////////////////

/**
 * Calculate the average of the population tours
 */
double average(std::vector<double> v) {
  if (v.empty())
    return 0;
  return std::accumulate(v.begin(), v.end(), 0) / static_cast<double>(v.size());
}

// Function to perform mutation by swapping two cities
void mutate(std::vector<int> &tour, int percentageForMutation, std::string type) {
  // select a percentage for mutation
  std::mt19937 gen({std::random_device{}()});
  std::uniform_int_distribution<> dis(0, 100);
  if (dis(gen) > percentageForMutation) return;

  int numCities = tour.size();
  std::uniform_int_distribution<> dis_cities(0, numCities - 1);
  int index1 = dis_cities(gen);
  int index2 = dis_cities(gen);
  if (type == "swap") std::swap(tour[index1], tour[index2]);
  else if (type == "reverse") std::reverse(tour.begin() + index1, tour.begin() + index2);
}

// Function to perform selection using tournament selection
std::vector<int> selection(const std::vector<std::vector<int>> &population,
                           const TSPInstance &instance, const int nCandidates) {
  std::mt19937 gen({std::random_device{}()});
  std::uniform_int_distribution<> dis(0, population.size() - 1);

  std::vector<int> selectedTour;
  double bestDistance = std::numeric_limits<double>::max();

  // Find random populations and check the best by using tournament selection
  for (int i = 0; i < nCandidates; ++i) {
    int index = dis(gen);
    const std::vector<int> &tour = population[index];
    double distance = instance.calcTourLength(tour);
    if (distance < bestDistance) {
      bestDistance = distance;
      selectedTour = tour;
    }
  }

  return selectedTour;
}

// Function to perform crossover using ordered crossover
std::vector<int> crossover(const std::vector<int> &parent1,
                           const std::vector<int> &parent2) {
  int numCities = parent1.size();
  std::vector<int> child(numCities, -1);
  std::mt19937 gen({std::random_device{}()});
  std::uniform_int_distribution<> dis(0, numCities - 1);
  int startPos = dis(gen);
  int endPos = dis(gen);

  for (int i = startPos; i <= endPos; ++i) {
    child[i] = parent1[i];
  }

  int parent2Index = 0;
  for (int i = 0; i < numCities; ++i) {
    if (find(child.begin(), child.end(), parent2[i]) == child.end()) {
      while (child[parent2Index] != -1) {
        ++parent2Index;
      }
      child[parent2Index] = parent2[i];
    }
  }

  return child;
}

void GAOptimizer::optimize(const TSPInstance &instance, std::vector<int> &result) const {
  // Get the number of cities
  int numCities = static_cast<int>(instance.getCities().size());

  // Generate initial population
  // std::vector<std::vector<int>> population(configSA.populationSize);
  for (int i = 0; i < populationSize_; ++i) {
    population_[i] = createRandomTour(numCities);
  }

  // Main loop for the specified number of generations
  for (int generation = 0; generation < numGenerations_; ++generation) {
    // Create a new population for the next generation
    std::vector<std::vector<int>> newPopulation(populationSize_);

    // Perform selection, crossover, and mutation to create the new population
    for (int i = 0; i < populationSize_; ++i) {
      std::vector<int> parent1 = selection(population_, instance,
                                           competitorsInTournament_);
      std::vector<int> parent2 = selection(population_, instance,
                                           competitorsInTournament_);
      std::vector<int> child = crossover(parent1, parent2);
      std::string type = "reverse"; // swap
      mutate(child, percentageForMutation_, type);
      newPopulation[i] = child;
    }

    // Replace the current population with the new population
    population_.clear();
    std::move(newPopulation.begin(), newPopulation.end(),
              std::back_inserter(population_));
    currentGenerationNumber_ = generation;

    // Find the best tour in the population and save all distances
    double bestDistance = std::numeric_limits<double>::max();
    std::vector<int> bestTour;
    // TODO(BORGES): Verify possibility of integrating this loop into the last one
    for (int i = 0; i < populationSize_; ++i) {
      auto tour = population_[i];
      double distance = instance.calcTourLength(tour);
      populationEnergies_[i] = distance;
      if (distance < bestDistance) {
        bestDistance = distance;
        bestTour = tour;
        state_ = bestTour;
        energy_ = bestDistance;
      }
    }

    if (energy_ < bestEnergy_) {
      bestState_ = state_;
      bestEnergy_ = energy_;
    }

    // Calculate the average of the population energies
    double avg = average(populationEnergies_);
    averagesPopulationEnergies_.push_back(avg);

    // Should we notify the observers?
    for (size_t i = 0; i < observers.size(); ++i) {
      observers_[i]->notify(instance, this);
    }
  }

  // Do the final notification
  terminated_ = true;
  state_ = bestState_;
  for (size_t i = 0; i < observers_.size(); ++i) {
    observers_[i]->notify(instance, this);
  }

  result = bestState_;
}