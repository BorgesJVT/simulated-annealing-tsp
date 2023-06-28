#ifndef RUNTIMEGUI_H
#define RUNTIMEGUI_H

#include <opencv2/opencv.hpp>
// #include <CvPlot/cvplot.h>

#include "optimizer.h"
#include "tsp.h"

/**
 * This class implements an observer for the optimizer. The observers can
 * watch the optimization process
 */
class Observer {
public:
  /**
   * This method notifies the observers passing references to the TSP instace
   * problem and the optimizer which is being used to solve.
   */
  virtual void notify(const TSPInstance &instance, Optimizer &optimizer) = 0;
};

/**
 * This is the runtime GUI that let's you watch what happens during the
 * optimization procedure
 */
class RuntimeGUI : public Observer {
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
                      Optimizer &optimizer) override;

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