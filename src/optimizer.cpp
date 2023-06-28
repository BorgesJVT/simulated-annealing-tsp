#include "optimizer.h"

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

// Brute force Dynamic Programming
void Optimizer::optimize(const TSPInstance& instance, std::vector<int> &
result, std::string brute_force) const
{
    // Start timer
    auto lastUpdate = std::chrono::system_clock::now();
    // Get the number of cities
    int numCities = static_cast<int>(instance.getCities().size());

    // Set up the runtime configuration
    Config configBF;

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
        configBF.state = tour;
        configBF.energy = tourLength;
        configBF.bestEnergy = minTourLength;
        configBF.bestState = optimalTour;
        observers[0]->notify(instance, configBF);
    }
    // compute time difference to start timer
    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();
    std::cout << "Elapsed time for optimization: " << timeSinceLastUpdate << "ms"
                << std::endl;
    std::cout << "Optimal tour length: " << minTourLength << std::endl;
}

/* SIMPLE GENETIC ALGORITHM */
// Function to create a random tour
std::vector<int> createRandomTour(int numCities) {
  std::vector<int> tour(numCities);
  for (int i = 0; i < numCities; ++i) {
    tour[i] = i;
  }
  std::mt19937 gen({std::random_device{}()});
  shuffle(tour.begin(), tour.end(), gen);
  return tour;
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

// void Optimizer::optimize(const TSPInstance &instance, std::vector<int> &result,
//                          std::string geneticAlgorithm) const {
//   // Set up the runtime configuration
//   ConfigSA configSA;

//   // Get the number of cities
//   int numCities = static_cast<int>(instance.getCities().size());

//   // Generate initial population
//   // std::vector<std::vector<int>> population(configSA.populationSize);
//   for (int i = 0; i < configSA.populationSize; ++i) {
//     configSA.population[i] = createRandomTour(numCities);
//   }

//   // Main loop for the specified number of generations
//   for (int generation = 0; generation < configSA.numGenerations; ++generation) {
//     // Create a new population for the next generation
//     std::vector<std::vector<int>> newPopulation(configSA.populationSize);

//     // Perform selection, crossover, and mutation to create the new population
//     for (int i = 0; i < configSA.populationSize; ++i) {
//       std::vector<int> parent1 = selection(configSA.population, instance,
//                                            configSA.competitorsInTournament);
//       std::vector<int> parent2 = selection(configSA.population, instance,
//                                            configSA.competitorsInTournament);
//       std::vector<int> child = crossover(parent1, parent2);
//       std::string type = "reverse"; // swap
//       mutate(child, configSA.percentageForMutation, type);
//       newPopulation[i] = child;
//     }

//     // Replace the current population with the new population
//     configSA.population.clear();
//     std::move(newPopulation.begin(), newPopulation.end(),
//               std::back_inserter(configSA.population));
//     configSA.currentGenerationNumber = generation;

//     // Find the best tour in the population and save all distances
//     double bestDistance = std::numeric_limits<double>::max();
//     std::vector<int> bestTour;
//     // TODO(BORGES): Verify possibility of integrating this loop into the last one
//     for (int i = 0; i<configSA.populationSize; ++i) {
//       auto tour = configSA.population[i];
//       double distance = instance.calcTourLength(tour);
//       configSA.populationEnergies[i] = distance;
//       if (distance < bestDistance) {
//         bestDistance = distance;
//         bestTour = tour;
//         configSA.state = bestTour;
//         configSA.energy = bestDistance;
//       }
//     }

//     if (configSA.energy < configSA.bestEnergy) {
//       configSA.bestState = configSA.state;
//       configSA.bestEnergy = configSA.energy;
//     }

//     // Calculate the average of the population energies
//     double avg = ConfigSA::average(configSA.populationEnergies);
//     configSA.averagesPopulationEnergies.push_back(avg);

//     // Should we notify the observers?
//     for (size_t i = 0; i < observers.size(); i++) {
//       observers[i]->notify(instance, configSA);
//     }
//   }

//   // Do the final notification
//   configSA.terminated = true;
//   configSA.state = configSA.bestState;
//   for (size_t i = 0; i < observers.size(); i++) {
//     observers[i]->notify(instance, configSA);
//   }

//   result = configSA.bestState;
// }
// Simulated Annealing
void Optimizer::optimize(const TSPInstance &instance,
                         std::vector<int> &result) const {
  // Get the number of cities
  int n = static_cast<int>(instance.getCities().size());

  assert(n > 0);
  // There has to be at least one move for the optimization to work
  assert(moves.size() > 0);

  // Set up the runtime configuration
  Config config;

  // Set up some initial tour
  config.state.resize(n);
  config.bestState.resize(n);
  for (int i = 0; i < n; i++) {
    config.state[i] = i;
  }

  // Shuffle the array randomly
  std::srand(unsigned(std::time(0)));
  std::random_shuffle(config.state.begin() + 1, config.state.end());

  config.energy = instance.calcTourLength(config.state);

  config.bestEnergy = config.energy;

  config.temp = coolingSchedule->initialTemp();

  std::mt19937 g({std::random_device{}()});
  // Set up an initial distribution over the possible moves
  std::uniform_int_distribution<int> moveDist(
      0, static_cast<int>(moves.size()) - 1);
  // A uniform distribution for the acceptance probability
  std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);

  // Set up the mover service
  Optimizer::MoveService *service = new Optimizer::MoveService(n);
  for (size_t i = 0; i < moves.size(); i++) {
    moves[i]->setMoveService(service);
  }

  // The current proposal/neighbor
  std::vector<int> proposal;

  // A total loop counter for the notification cycle
  int loopCounter = 0;
  // Start timer
  auto lastUpdate = std::chrono::system_clock::now();

  // Start the optimization
  for (config.outer = 0; config.outer < outerLoops; config.outer++) {
    // Determine the next temperature
    config.temp = coolingSchedule->nextTemp(config);

    // Simulate the markov chain
    for (config.inner = 0; config.inner < innerLoops; config.inner++) {
      proposal = config.state;

      // Propose a new neighbor according to some move
      // Choose the move
      int m = moveDist(g);
      moves[m]->propose(proposal);

      // Get the energy of the new proposal
      config.proposedEnergy = instance.calcTourLength(proposal);
      const float delta = config.proposedEnergy - config.energy;

      // Did we decrease the energy?
      if (delta <= 0) {
        // Accept the move
        config.state = proposal;
        config.energy = config.proposedEnergy;
      } else {
        // Accept the proposal with a certain probability
        float u = uniformDist(g);
        if (u <= std::exp(-1 / config.temp * delta)) {
          config.state = proposal;
          config.energy = config.proposedEnergy;
        }
      }

      // Is this better than the best global optimum?
      if (config.proposedEnergy < config.bestEnergy) {
        // It is
        config.bestEnergy = config.proposedEnergy;
        config.bestState = proposal;
      }

      // Should we notify the observers?
      if ((loopCounter % notificationCycle) == 0) {
        // Yes, we should
        for (size_t i = 0; i < observers.size(); i++) {
          observers[i]->notify(instance, config);
        }
      }
      loopCounter++;
      config.proposedEnergies.push_back(config.proposedEnergy);
    } // end inner loop (markov chain)
  }   // end outer loop
  // compute time difference to start timer
  long timeSinceLastUpdate =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now() - lastUpdate)
          .count();
  std::cout << "Elapsed time for optimization: " << timeSinceLastUpdate << "ms"
            << std::endl;
  // config.timeExecution = timeSinceLastUpdate;

  // Unregister the move service
  DELETE_PTR(service);
  for (size_t i = 0; i < moves.size(); i++) {
    moves[i]->setMoveService(0);
  }

  result = config.bestState;

  // Do the final notification
  config.terminated = true;
  config.state = config.bestState;
  config.energy = config.bestEnergy;
  for (size_t i = 0; i < observers.size(); i++) {
    observers[i]->notify(instance, config);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// RuntimeGUI
////////////////////////////////////////////////////////////////////////////////

void RuntimeGUI::notify(const TSPInstance &instance,
                        const Optimizer::ConfigSA &configSA) {
  // The screen is split as follows:
  // 75% points
  // 25% status

  // Clear the gui
  gui = cv::Scalar(0);

  // Get the status marker
  int statusRow = 0.05 * gui.rows;
  int statusCol = 0.40 * gui.cols;
  int line_xcoord = 120;
  int line_ycoord = 15;

  // Write the status
  // Beside Graph
  std::stringstream ss;
  // ss << "Population: ";
  // cv::putText(gui, ss.str(), cv::Point(statusCol + line_xcoord, statusRow),
  //             cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(0, 255, 0));

  // for (auto tour : configSA.population) {
  //   ss.str("");
  //   for (auto e : tour) {
  //     ss << e << " ";
  //   }
  //   cv::putText(gui, ss.str(),
  //               cv::Point(statusCol + line_xcoord, statusRow + line_ycoord),
  //               cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 255, 0));
  //   line_ycoord = line_ycoord + 15;
  //   if (line_ycoord > statusCol)
  //     break;
  // }

  // Below Graph
  ss.str("");
  ss << "Generation # = " << configSA.currentGenerationNumber;
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 30),
              cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
  ss.str("");
  ss << "Current distance = " << configSA.energy;
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 60),
              cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
  ss.str("");
  ss << "Shortest distance = " << configSA.bestEnergy;
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 90),
              cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
  ss.str("");
  ss << "best state = [ ";
  for (auto e : configSA.bestState) {
    ss << e << " ";
  }
  ss << "]";
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 120),
              cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));
  ss.str("");
  // ss << "Average of the population distances: " << configSA.avgPopulationEnergies;
  // cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 150),
  //             cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(255, 255, 255));

  // Plot the charts
  // [...]

  // Plot the cities
  // Determine the minimum and maximum X/Y
  float minX = std::numeric_limits<float>::max();
  float minY = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::min();
  float maxY = std::numeric_limits<float>::min();

  for (size_t i = 0; i < instance.getCities().size(); i++) {
    minX = std::min(minX, instance.getCities()[i].second);
    minY = std::min(minY, instance.getCities()[i].first);
    maxX = std::max(maxX, instance.getCities()[i].second);
    maxY = std::max(maxY, instance.getCities()[i].first);
  }

  // Calculate the compression factor
  float width = maxX - minX;
  float height = maxY - minY;
  float compression = (statusCol - 10) / width;
  if (height * compression > gui.rows - 10) {
    compression = (gui.rows - 10) / height;
  }

  // Paint the best path
  for (size_t i = 0; i < configSA.state.size(); i++) {
    cv::Point p1;
    p1.x = (instance.getCities()[configSA.bestState[i % configSA.state.size()]]
                .second -
            minX) *
               compression +
           5;
    p1.y = (instance.getCities()[configSA.bestState[i % configSA.state.size()]]
                .first -
            minY) *
               compression +
           5;
    cv::Point p2;
    p2.x =
        (instance
             .getCities()[configSA.bestState[(i + 1) % configSA.state.size()]]
             .second -
         minX) *
            compression +
        5;
    p2.y =
        (instance
             .getCities()[configSA.bestState[(i + 1) % configSA.state.size()]]
             .first -
         minY) *
            compression +
        5;

    cv::line(gui, p1, p2, cv::Scalar(0, 255, 255), 1,
             CV_AVX); // test: cv::LINE_AA
  }
  // Paint the current path
  for (size_t i = 0; i < configSA.state.size(); i++) {
    cv::Point p1;
    p1.x = (instance.getCities()[configSA.state[i % configSA.state.size()]]
                .second -
            minX) *
               compression +
           5;
    p1.y =
        (instance.getCities()[configSA.state[i % configSA.state.size()]].first -
         minY) *
            compression +
        5;
    cv::Point p2;
    p2.x =
        (instance.getCities()[configSA.state[(i + 1) % configSA.state.size()]]
             .second -
         minX) *
            compression +
        5;
    p2.y =
        (instance.getCities()[configSA.state[(i + 1) % configSA.state.size()]]
             .first -
         minY) *
            compression +
        5;

    cv::line(gui, p1, p2, cv::Scalar(255, 0, 255), 2,
             CV_AVX); // test:cv::LINE_AA
  }

  // Paint the cities
  for (size_t i = 0; i < instance.getCities().size(); i++) {
    cv::Point p1;
    p1.x = (instance.getCities()[i].second - minX) * compression + 5;
    p1.y = (instance.getCities()[i].first - minY) * compression + 5;

    cv::circle(gui, p1, 2, cv::Scalar(200, 200, 200), 2);
  }

  cv::imshow("TSP", gui);

  // auto energyPlot = CvPlot::plot(configSA.averagesPopulationEnergies);
  // cv::Mat energyPlotMat = energyPlot.render(500, 1000);
  // cv::imshow("Energy Plot", energyPlotMat);

  if (configSA.terminated) {
    cv::waitKey(0);
  } else {
    cv::waitKey(waitTime);
  }
}

void RuntimeGUI::notify(const TSPInstance &instance,
                        const Optimizer::Config &config) {
  // The screen is split as follows:
  // 75% points
  // 25% status

  // Clear the gui
  gui = cv::Scalar(0);

  // Get the status marker
  int statusRow = 0.05 * gui.rows;
  int statusCol = 0.75 * gui.cols;

  // Write the status
  std::stringstream ss;
  ss << "temp = " << config.temp;
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 15),
              cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  ss.str("");
  ss << "outer = " << config.outer;
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 30),
              cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  ss.str("");
  ss << "inner = " << config.inner;
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 45),
              cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  ss.str("");
  ss << "energy = " << config.proposedEnergy;
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 60),
              cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  ss.str("");
  ss << "best energy = " << config.bestEnergy;
  cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 75),
              cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  ss.str("");
  // ss << "best state = [" << config.bestState[0] << "  " <<
  // config.bestState[1] << "  " << config.bestState[2] << "  " <<
  // config.bestState[3] << "]"; cv::putText(    gui,
  //                 ss.str(),
  //                 cv::Point(statusRow, statusCol+90),
  //                 cv::FONT_HERSHEY_PLAIN,
  //                 0.9,
  //                 cv::Scalar(255,255,255));
  // ss.str("");

  // Plot the charts
  // [...]

  // Plot the cities
  // Determine the minimum and maximum X/Y
  float minX = std::numeric_limits<float>::max();
  float minY = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::min();
  float maxY = std::numeric_limits<float>::min();

  for (size_t i = 0; i < instance.getCities().size(); i++) {
    minX = std::min(minX, instance.getCities()[i].second);
    minY = std::min(minY, instance.getCities()[i].first);
    maxX = std::max(maxX, instance.getCities()[i].second);
    maxY = std::max(maxY, instance.getCities()[i].first);
  }

  // Calculate the compression factor
  float width = maxX - minX;
  float height = maxY - minY;
  float compression = (statusCol - 10) / width;
  if (height * compression > gui.rows - 10) {
    compression = (gui.rows - 10) / height;
  }

  // Paint the best path
  for (size_t i = 0; i < config.state.size(); i++) {
    cv::Point p1;
    p1.x = (instance.getCities()[config.bestState[i % config.state.size()]]
                .second -
            minX) *
               compression +
           5;
    p1.y =
        (instance.getCities()[config.bestState[i % config.state.size()]].first -
         minY) *
            compression +
        5;
    cv::Point p2;
    p2.x =
        (instance.getCities()[config.bestState[(i + 1) % config.state.size()]]
             .second -
         minX) *
            compression +
        5;
    p2.y =
        (instance.getCities()[config.bestState[(i + 1) % config.state.size()]]
             .first -
         minY) *
            compression +
        5;

    cv::line(gui, p1, p2, cv::Scalar(0, 255, 255), 1,
             CV_AVX); // test: cv::LINE_AA
  }
  // Paint the current path
  for (size_t i = 0; i < config.state.size(); i++) {
    cv::Point p1;
    p1.x = (instance.getCities()[config.state[i % config.state.size()]].second -
            minX) *
               compression +
           5;
    p1.y = (instance.getCities()[config.state[i % config.state.size()]].first -
            minY) *
               compression +
           5;
    cv::Point p2;
    p2.x = (instance.getCities()[config.state[(i + 1) % config.state.size()]]
                .second -
            minX) *
               compression +
           5;
    p2.y = (instance.getCities()[config.state[(i + 1) % config.state.size()]]
                .first -
            minY) *
               compression +
           5;

    cv::line(gui, p1, p2, cv::Scalar(255, 0, 255), 2,
             CV_AVX); // test: cv::LINE_AA
  }

  // Paint the cities
  for (size_t i = 0; i < instance.getCities().size(); i++) {
    cv::Point p1;
    p1.x = (instance.getCities()[i].second - minX) * compression + 5;
    p1.y = (instance.getCities()[i].first - minY) * compression + 5;

    cv::circle(gui, p1, 2, cv::Scalar(200, 200, 200), 2);
  }

  cv::imshow("TSP", gui);

  // auto energyPlot = CvPlot::plot(config.proposedEnergies);
  // cv::Mat energyPlotMat = energyPlot.render(1000, 1500);
  // cv::imshow("Energy Plot", energyPlotMat);

  if (config.terminated) {
    cv::waitKey(0);
  } else {
    cv::waitKey(waitTime);
  }
}