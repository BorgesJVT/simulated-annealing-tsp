#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <CvPlot/cvplot.h>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <random>
#include <numeric>

#include "tsp.h"

/**
 * This is the optimizer. It implements the basic simulated annealing algorithm
 * and several neighborhood moves.
 */
class Optimizer {
public:
  class ConfigSA {
  public:
    ConfigSA()
        : populationSize(1000), population(populationSize),
          populationEnergies(populationSize), numGenerations(500),
          currentGenerationNumber(0), energy(0),
          bestEnergy(std::numeric_limits<float>::max()),
          competitorsInTournament(3), percentageForMutation(100),
          terminated(false) {}
    
    /**
     * Calculate the average of the population tours
     */
    static double average(std::vector<double> v) {
      if (v.empty())
        return 0;
      return std::reduce(v.begin(), v.end()) / static_cast<double>(v.size());
    }
    /**
     * The population size
     */
    int populationSize;
    /**
     * The population
     */
    std::vector<std::vector<int>> population;
    /**
     * The population distances
     */
    std::vector<double> populationEnergies;
    /**
     * The averages of the population distances
     */
    std::vector<double> averagesPopulationEnergies;
    /**
     * The best tour index
     */
    int bestTourIndex;
    /**
     * The number of generations
     */
    int numGenerations;
    /**
     * The current generation in execution
     */
    int currentGenerationNumber;
    /**
     * The number of competitors for selection by tournament
     */
    int competitorsInTournament;
    /**
     * Must be an integer from 0 - 100 defining a percentage for mutation
     */
    int percentageForMutation;
    /**
     * The current distance
     */
    float energy;
    /**
     * The best energy: shortest distance so far
     */
    float bestEnergy;
    /**
     * Current state
     */
    std::vector<int> state;
    /**
     * The best state observed so far
     */
    std::vector<int> bestState;
    /**
     * Whether or not the system has terminated
     */
    bool terminated;
  };

  /**
   * The runtime configuration of the algorithm
   */
  class Config {
  public:
    Config()
        : temp(0), outer(0), inner(0), energy(0), bestEnergy(0),
          terminated(false) {}
    /**
     * The current temperature
     */
    float temp;
    /**
     * The current outer loop
     */
    int outer;
    /**
     * The current inner loop
     */
    int inner;
    /**
     * The current objective
     */
    float energy;
    /**
     * The current proposed energy (this variable is just for plotting in GUI).
     * You can remove it from the code without losses.
     */
    float proposedEnergy;
    /**
     * The energies proposed
     */
    std::vector<float> proposedEnergies;
    /**
     * The currently best energy found
     */
    float bestEnergy;
    /**
     * Current state
     */
    std::vector<int> state;
    /**
     * The best state observed so far
     */
    std::vector<int> bestState;
    /**
     * Whether or not the system has terminated
     */
    bool terminated;
    /**
     * Time execution spent to run the optimizer
     */
    // long timeExecution{0};
  };

  /**
   * This class implements an observer for the optimizer. The observers can
   * watch the optimization process
   */
  class Observer {
  public:
    /**
     * This method is called by the optimizer
     */
    virtual void notify(const TSPInstance &instance,
                        const ConfigSA &config) = 0;
    virtual void notify(const TSPInstance &instance, const Config &config) = 0;
  };

  /**
   * This class defines a cooling schedule
   */
  class CoolingSchedule {
  public:
    /**
     * Calculates the next temperature
     */
    virtual float nextTemp(const Config &config) const = 0;
    /**
     * Returns the initial temperature
     */
    virtual float initialTemp() const = 0;
  };

  /**
   * This is a move service class that allows the random sampling of cities
   */
  class MoveService {
  public:
    /**
     * Constructor
     */
    MoveService(int numCities)
        : generator({std::random_device{}()}), distribution(1, numCities - 1) {}

    /**
     * Returns a random city
     */
    int sample() { return distribution(generator); }

  private:
    /**
     * The random number generator
     */
    std::mt19937 generator;
    /**
     * The distribution over the cities
     */
    std::uniform_int_distribution<int> distribution;
  };

  /**
   * This class implements a single neighborhood move
   */
  class Move {
  public:
    /**
     * Computes a random neighbor according to some move strategy
     */
    virtual void propose(std::vector<int> &state) const = 0;

    /**
     * Sets the move service
     */
    void setMoveService(MoveService *_service) { service = _service; }

  protected:
    /**
     * The move service
     */
    MoveService *service;
  };

  /**
   * Constructor
   */
  Optimizer()
      : coolingSchedule(0), outerLoops(100), innerLoops(1000),
        notificationCycle(250) {}

  /**
   * The cooling schedule
   */
  CoolingSchedule *coolingSchedule;
  /**
   * The number of outer iterations
   */
  int outerLoops;
  /**
   * The number of inner iterations
   */
  int innerLoops;
  /**
   * The notification cycle. Every c iterations, the observers are notified
   */
  int notificationCycle;
  /**
   * Runs the optimizer on a specific problem instance using Brute Force Dynamic
   * Programming
   */
  // void optimize(const TSPInstance& instance, std::vector<int> & result,
  // std::string brute_force) const;
  /**
   * Runs the optimizer on a specific problem instance using Simulated Annealing
   */
  void optimize(const TSPInstance &instance, std::vector<int> &result) const;

  /**
   * Adds an observer
   */
  void addObserver(Observer *observer) { observers.push_back(observer); }

  /**
   * Adds a move
   */
  void addMove(Move *move) { moves.push_back(move); }

  // Simple Genetic Algorithm
  void optimize(const TSPInstance &instance, std::vector<int> &result,
                std::string geneticAlgorithm) const;

private:
  /**
   * A list of observers
   */
  std::vector<Observer *> observers;
  /**
   * A list of move classes
   */
  std::vector<Move *> moves;
}; // end of Optimizer class

/**
 * This is a geometric cooling schedule
 */
class GeometricCoolingSchedule : public Optimizer::CoolingSchedule {
public:
  /**
   * Constructor
   */
  GeometricCoolingSchedule(float initialTemp, float endTemp, float alpha)
      : iTemp(initialTemp), eTemp(endTemp), alpha(alpha) {}

  /**
   * Calculates the next temperature
   */
  virtual float nextTemp(const Optimizer::Config &config) const {
    return std::max(config.temp * alpha, eTemp);
  }

  /**
   * Returns the initial temperature
   */
  virtual float initialTemp() const { return iTemp; }

private:
  /**
   * The initial temperature
   */
  float iTemp;
  /**
   * End temperature
   */
  float eTemp;
  /**
   * alpha -> Decreasing constant
   */
  float alpha;
};

/**
 * This move reverses the order of a chain
 */
class ChainReverseMove : public Optimizer::Move {
public:
  /**
   * Computes a random neighbor according to some move strategy
   */
  virtual void propose(std::vector<int> &state) const {
    // Sample two random cities and reverse the chain
    std::reverse(state.begin() + service->sample(),
                 state.begin() + service->sample());
  }
};

/**
 * This move exchanges two cities in the current path
 */
class SwapCityMove : public Optimizer::Move {
public:
  /**
   * Computes a random neighbor according to some move strategy
   */
  virtual void propose(std::vector<int> &state) const {
    std::swap(state[service->sample()], state[service->sample()]);
  }
};

/**
 * This move rotates the current path
 */
class RotateCityMove : public Optimizer::Move {
public:
  /**
   * Computes a random neighbor according to some move strategy
   */
  virtual void propose(std::vector<int> &state) const {
    std::vector<int> c(
        {service->sample(), service->sample(), service->sample()});
    std::sort(c.begin(), c.end());

    std::rotate(state.begin() + c[0], state.begin() + c[1],
                state.begin() + c[2]);
  }
};

/**
 * This is the runtime GUI that let's you watch what happens during the
 * optimization procedure
 */
class RuntimeGUI : public Optimizer::Observer {
public:
  /**
   * Constructor
   */
  RuntimeGUI(int rows, int cols) : waitTime(25), gui(rows, cols, CV_8UC3) {
    // Open the window
    cv::namedWindow("TSP", 1);
  }

  /**
   * Destructor
   */
  virtual ~RuntimeGUI() { cv::destroyWindow("TSP"); }

  /**
   * Paint the gui
   */
  virtual void notify(const TSPInstance &instance,
                      const Optimizer::ConfigSA &configSA) override;
  virtual void notify(const TSPInstance &instance,
                      const Optimizer::Config &config) override;

  /**
   * The time the GUI pauses after each update. Set to 0 to let
   * it wait for a keypress
   */
  int waitTime;

private:
  /**
   * The GUI matrix
   */
  cv::Mat gui;
  // CvPlot::Axes energyPlot;
}; // end of RuntimeGUI class
#endif