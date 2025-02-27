#include <map>
#include <string>
#include "tsp.h"
#include "optimizer.h"
#include "runtimegui.h"

int main(int argc, const char** argv)
{
    // Set up a random problem instance 
    TSPInstance instance;
    if (argc > 1)
    {
        std::ifstream stream;
        stream.open(argv[1]);
        if(!stream.is_open())
        {
            std::cout << "Cannot open data file.";
            return 1;
        }
        instance.readTSPLIB(stream);
        stream.close();
    }
    else
    {
        instance.createCircleOfCities(8);
    }
    instance.calcDistanceMatrix();
    
    // Set up the optimizer 
    Optimizer optimizer;
    
    // Register the GUI
    // You can specify the dimensions of the window
    RuntimeGUI gui(850, 1700);
    // The time the GUI stops after each iterations. Set to 0 to wait for a keypress
    gui.waitTime = 1;
    optimizer.addObserver(&gui);


    // Register the moves
    // SwapCityMove move;
    // optimizer.addMove(&move);
    
    
    // Choose a cooling schedule
    // GeometricCoolingSchedule schedule(100, 1e-2, 0.99);
    // optimizer.coolingSchedule = &schedule;
    
    // Optimizer loop counts
    // optimizer.outerLoops = 300;
    // optimizer.innerLoops = 100;
    // Update the GUI every x iterations
    // optimizer.notificationCycle = 1;
    
    // Run the program
    std::vector<int> result;
    optimizer.optimize(instance, result);
    
    return 0;
}