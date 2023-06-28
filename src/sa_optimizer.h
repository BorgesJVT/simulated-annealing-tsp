#ifndef SA_OPTIMIZER_H
#define SA_OPTIMIZER_H

#include "optimizer.h"

/**
 * The class SAOptimizer implements the basic simulated annealing algorithm
 * and with the possibility of several neighborhood moves.
 */
class SAOptimizer : public Optimizer {
public:
  /**
   * This class defines a cooling schedule
   */
  class CoolingSchedule {
  public:
    /**
     * Calculates the next temperature
     */
    virtual float nextTemp(const float temp) const = 0;
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
    void setMoveService(MoveService *service) { service_ = service; }

  protected:
    /**
     * The move service
     */
    MoveService *service_;
  };
  /**
   * The current proposed energy (this variable is just for plotting in GUI).
   * You can remove it from the code without losses.
   */
  float proposedEnergy_;
  /**
   * The energies proposed
   */
  std::vector<float> proposedEnergies_;
  /**
   * The current temperature
   */
  float temp_;
  /**
   * The cooling schedule
   */
  CoolingSchedule *coolingSchedule_;
  /**
   * The number of outer iterations
   */
  int outerLoops_;
  /**
   * The current outer loop
   */
  int outer_;
  /**
   * The number of inner iterations
   */
  int innerLoops_;
  /**
   * The current inner loop
   */
  int inner_;
  /**
   * The notification cycle. Every c iterations, the observers are notified
   */
  int notificationCycle_;
  /**
   * A list of move classes
   */
  std::vector<Move *> moves_;
  /**
   * Constructor
   */
  SAOptimizer()
      : coolingSchedule_(0), outerLoops_(100), innerLoops_(1000),
        notificationCycle_(250) {}
  void optimize(const TSPInstance &instance, std::vector<int> &result) override;
  /**
   * Adds a move
   */
  void addMove(Move *move) { moves_.push_back(move); }
}; // end of SAOptimizer class

/**
 * This is a geometric cooling schedule
 */
class GeometricCoolingSchedule : public SAOptimizer::CoolingSchedule {
public:
  /**
   * Constructor
   */
  GeometricCoolingSchedule(float initialTemp, float endTemp, float alpha)
      : iTemp(initialTemp), eTemp(endTemp), alpha(alpha) {}

  /**
   * Calculates the next temperature
   */
  virtual float nextTemp(const float temp) const {
    return std::max(temp * alpha, eTemp);
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
class ChainReverseMove : public SAOptimizer::Move {
public:
  /**
   * Computes a random neighbor according to some move strategy
   */
  virtual void propose(std::vector<int> &state) const {
    // Sample two random cities and reverse the chain
    std::reverse(state.begin() + service_->sample(),
                 state.begin() + service_->sample());
  }
};

/**
 * This move exchanges two cities in the current path
 */
class SwapCityMove : public SAOptimizer::Move {
public:
  /**
   * Computes a random neighbor according to some move strategy
   */
  virtual void propose(std::vector<int> &state) const {
    std::swap(state[service_->sample()], state[service_->sample()]);
  }
};

/**
 * This move rotates the current path
 */
class RotateCityMove : public SAOptimizer::Move {
public:
  /**
   * Computes a random neighbor according to some move strategy
   */
  virtual void propose(std::vector<int> &state) const {
    std::vector<int> c(
        {service_->sample(), service_->sample(), service_->sample()});
    std::sort(c.begin(), c.end());

    std::rotate(state.begin() + c[0], state.begin() + c[1],
                state.begin() + c[2]);
  }
};
#endif