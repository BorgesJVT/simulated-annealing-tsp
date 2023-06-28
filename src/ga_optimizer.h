#ifndef GA_OPTIMIZER_H
#define GA_OPTIMIZER_H

#include "optimizer.h"

/**
 * The class GAOptimizer implements simple genetic algorithm
 * and with different mutation moves.
 */
class GAOptimizer : public Optimizer {
public:
  /**
   * The population size
   */
  int populationSize_;
  /**
   * The population
   */
  std::vector<std::vector<int>> population_;
  /**
   * The population distances
   */
  std::vector<double> populationEnergies_;
  /**
   * The averages of the population distances
   */
  std::vector<double> averagesPopulationEnergies_;
  /**
   * The best tour index
   */
  int bestTourIndex_;
  /**
   * The number of generations
   */
  int numGenerations_;
  /**
   * The current generation in execution
   */
  int currentGenerationNumber_;
  /**
   * The number of competitors for selection by tournament
   */
  int competitorsInTournament_;
  /**
   * Must be an integer from 0 - 100 defining a percentage for mutation
   */
  int percentageForMutation_;

  /**
   * Constructor
   */
  GAOptimizer()
      : populationSize_(1000), population_(populationSize),
        populationEnergies_(populationSize_), numGenerations_(300),
        currentGenerationNumber_(0), energy_(0),
        bestEnergy_(std::numeric_limits<float>::max()),
        competitorsInTournament_(10), percentageForMutation_(100),
        terminated_(false) {}
  void optimize(const TSPInstance &instance, std::vector<int> &result) const override;
}; // end of GAOptimizer class
#endif