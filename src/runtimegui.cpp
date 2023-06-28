#include "runtimegui.h"
#include "optimizer.h"

////////////////////////////////////////////////////////////////////////////////
/// RuntimeGUI
////////////////////////////////////////////////////////////////////////////////

void RuntimeGUI::notify(const TSPInstance &instance, Optimizer &optimizer) {
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
  // ss << "temp = " << config.temp;
  // cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 15),
  //             cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  // ss.str("");
  // ss << "outer = " << config.outer;
  // cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 30),
  //             cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  // ss.str("");
  // ss << "inner = " << config.inner;
  // cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 45),
  //             cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  // ss.str("");
  // ss << "energy = " << config.proposedEnergy;
  // cv::putText(gui, ss.str(), cv::Point(statusRow, statusCol + 60),
  //             cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(255, 255, 255));
  // ss.str("");
  ss << "best energy = " << optimizer.bestEnergy_;
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
  for (size_t i = 0; i < optimizer.state_.size(); i++) {
    cv::Point p1;
    p1.x = (instance.getCities()[optimizer.bestState_[i % optimizer.state_.size()]]
                .second -
            minX) *
               compression +
           5;
    p1.y =
        (instance.getCities()[optimizer.bestState_[i % optimizer.state_.size()]].first -
         minY) *
            compression +
        5;
    cv::Point p2;
    p2.x =
        (instance.getCities()[optimizer.bestState_[(i + 1) % optimizer.state_.size()]]
             .second -
         minX) *
            compression +
        5;
    p2.y =
        (instance.getCities()[optimizer.bestState_[(i + 1) % optimizer.state_.size()]]
             .first -
         minY) *
            compression +
        5;

    cv::line(gui, p1, p2, cv::Scalar(0, 255, 255), 1,
             CV_AVX); // test: cv::LINE_AA
  }
  // Paint the current path
  for (size_t i = 0; i < optimizer.state_.size(); i++) {
    cv::Point p1;
    p1.x = (instance.getCities()[optimizer.state_[i % optimizer.state_.size()]].second -
            minX) *
               compression +
           5;
    p1.y = (instance.getCities()[optimizer.state_[i % optimizer.state_.size()]].first -
            minY) *
               compression +
           5;
    cv::Point p2;
    p2.x = (instance.getCities()[optimizer.state_[(i + 1) % optimizer.state_.size()]]
                .second -
            minX) *
               compression +
           5;
    p2.y = (instance.getCities()[optimizer.state_[(i + 1) % optimizer.state_.size()]]
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

  if (optimizer.terminated_) {
    cv::waitKey(0);
  } else {
    cv::waitKey(waitTime);
  }
}