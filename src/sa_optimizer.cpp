#include "optimizer.h"

////////////////////////////////////////////////////////////////////////////////
/// Simulated Annealing Optimizer
////////////////////////////////////////////////////////////////////////////////


void SAOptimizer::optimize(const TSPInstance &instance,
                         std::vector<int> &result) const {
  // Get the number of cities
  int n = static_cast<int>(instance.getCities().size());

  assert(n > 0);
  // There has to be at least one move for the optimization to work
  assert(moves.size() > 0);

  // Set up some initial tour
  state_.resize(n);
  bestState_.resize(n);
  for (int i = 0; i < n; i++) {
    state_[i] = i;
  }

  // Shuffle the array randomly
  std::srand(unsigned(std::time(0)));
  std::random_shuffle(state_.begin() + 1, state_.end());

  energy_ = instance.calcTourLength(state_);

  bestEnergy_ = energy_;

  temp_ = coolingSchedule_->initialTemp();

  std::mt19937 g({std::random_device{}()});
  // Set up an initial distribution over the possible moves
  std::uniform_int_distribution<int> moveDist(
      0, static_cast<int>(moves_.size()) - 1);
  // A uniform distribution for the acceptance probability
  std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);

  // Set up the mover service
  Optimizer::MoveService *service = new Optimizer::MoveService(n);
  for (size_t i = 0; i < moves_.size(); i++) {
    moves_[i]->setMoveService(service);
  }

  // The current proposal/neighbor
  std::vector<int> proposal;

  // A total loop counter for the notification cycle
  int loopCounter = 0;
  // Start timer
  auto lastUpdate = std::chrono::system_clock::now();

  // Start the optimization
  for (outer_ = 0; outer_ < outerLoops_; outer_++) {
    // Determine the next temperature
    temp_ = coolingSchedule_->nextTemp(temp_);

    // Simulate the markov chain
    for (inner_ = 0; inner_ < innerLoops_; inner_++) {
      proposal = state_;

      // Propose a new neighbor according to some move
      // Choose the move
      int m = moveDist(g);
      moves_[m]->propose(proposal);

      // Get the energy of the new proposal
      proposedEnergy_ = instance.calcTourLength(proposal);
      const float delta = proposedEnergy_ - energy_;

      // Did we decrease the energy?
      if (delta <= 0) {
        // Accept the move
        state_ = proposal;
        energy_ = proposedEnergy_;
      } else {
        // Accept the proposal with a certain probability
        float u = uniformDist(g);
        if (u <= std::exp(-1 / temp_ * delta)) {
          state_ = proposal;
          energy_ = proposedEnergy_;
        }
      }

      // Is this better than the best global optimum?
      if (proposedEnergy_ < bestEnergy_) {
        // It is
        bestEnergy_ = proposedEnergy_;
        bestState_ = proposal;
      }

      // Should we notify the observers?
      if ((loopCounter % notificationCycle_) == 0) {
        // Yes, we should
        for (size_t i = 0; i < observers_.size(); ++i) {
          observers_[i]->notify(instance, this);
        }
      }
      loopCounter++;
      proposedEnergies_.push_back(proposedEnergy_);
    } // end inner loop (markov chain)
  }   // end outer loop
  // compute time difference to start timer
  long timeSinceLastUpdate =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now() - lastUpdate)
          .count();
  std::cout << "Elapsed time for optimization: " << timeSinceLastUpdate << "ms"
            << std::endl;
  // timeExecution_ = timeSinceLastUpdate;

  // Unregister the move service
  DELETE_PTR(service);
  for (size_t i = 0; i < moves_.size(); ++i) {
    moves_[i]->setMoveService(0);
  }

  result = bestState_;

  // Do the final notification
  terminated_ = true;
  state_ = bestState_;
  energy_ = bestEnergy_;
  for (size_t i = 0; i < observers_.size(); ++i) {
    observers_[i]->notify(instance, this);
  }
}
