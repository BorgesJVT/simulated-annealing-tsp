#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <algorithm>
#include <numeric>
#include <random>
#include <thread>

#include "tsp.h"

class Observer;
/**
 * The class Optimizer implements a basic Brute Force algorithm
 * testing all possible tour with an exponential algorithimic complexity.
 */
class Optimizer {
public:
  /**
   * The current distance
   */
  float energy_;
  /**
   * The currently best energy found: shortest distance so far
   */
  float bestEnergy_;
  /**
   * Current state
   */
  std::vector<int> state_;
  /**
   * The best state observed so far
   */
  std::vector<int> bestState_;
  /**
   * Whether or not the system has terminated
   */
  bool terminated_;
  /**
   * Time execution spent to run the optimizer
   */
  long timeExecution_;
  /**
   * Optimizer constructor
  */
  Optimizer()
      : energy_(0.0f), bestEnergy_(std::numeric_limits<float>::max()),
        terminated_(false), timeExecution_(0) {}
  /**
   * Runs the optimizer on a specific problem instance using Brute Force
   */
  virtual void optimize(const TSPInstance &instance,
                        std::vector<int> &result);

  /**
   * Adds an observer
   */
  void addObserver(Observer *observer) { observers_.push_back(observer); }
private:
  /**
   * A list of observers
   */
  std::vector<Observer *> observers_;
  // std::map parameters;
}; // end of Optimizer class
#endif