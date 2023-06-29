# SOLVING TSP PROBLEM

This code shows how the traveling salesman problem can be solved applying different optimization algorithms.



## How to compile the project?

First, install all the required dependencies. You only need to install OpenCV.

Then, create a build directory
```
$ mkdir build
$ cd build
```

Run cmake
```
$ cmake ..
```

Run make
```
$ make
```

The executable is named "solving_tsp". Run the 
program as follows
```
$ ./solving_tsp
```
## What problem do we solve?

If you run the program without any parameters, then a number of 9 cities distributed
over a circle is generated. However, you can test different datasets from the LIBTSP repository [1]. 
Simply provide the filename of the .tsp file as the first argument. You 
almost surely have to adjust the parameters in order to get a good approximation.

For a quick test, you can run an example from the data folder, like the following:
```
$ ./solving_tsp ../data/wi29.tsp
```
At the end, the program gives you the shortest path found.
You can compare your results (using your parameters settings) to the optimal result [2]. 

The program only works with instances of type TSP and edge weight type EUC_2D. 

## How do I change the parameters?

All important parameters are defined in the main.cpp file. 
The project has three optimization methods available, which are:
* _Brute force method_: a simple brute force code that tests all possible tours. No parameters required.
* _Simulated Annealing method_ (default):
  * outerLoops_: number of iterations in the external loop for temperature decayment.
  * innerLoops_: number of iterations in the internal loop for simulate the markov chain with a fixed temperature.
  * coolingSchedule_: defines the cooling process using a geometric cooling method with an _initial temperature_ and _decayment factor_ as parameters.
  * notificationCycle_: represents the screen update cycle for notifying the GUI.
* _Genetic Algorithm method_:
  * populationSize: size of population.
  * numGenerations: number of generations.
  * competitorsInTournament: number of competitors in a selection process by tournament.
  * percentageForMutation: a number which must be between 0 and 100 representing the percentage for mutation. 
  
You must choose just one method and comment the other ones. 
Remember to recompile the project once you are finished updating the parameters.
**Warning: The brute force method does not work well for a high number of cities (>10).**

## What do the lines represent?

The yellow line shows the a possible new cycle that has been found so far. The purple
line shows the current best state. 

# License
The MIT License (MIT) Copyright (c) 2023 João Victor Borges
The MIT License (MIT) Copyright (c) 2016 Tobias Pohlen

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Rubric Points Addressed
* **The project demonstrates an understanding of C++ functions and control structures.**
* **The project reads data from a file and process the data, or the program writes data to a file.**
* **The project accepts user input and processes the input.**
* **The project uses Object Oriented Programming techniques.**
* **Classes use appropriate access specifiers for class members.**
* **Class constructors utilize member initialization lists.**
* **Classes abstract implementation details from their interfaces.**
* **Classes encapsulate behavior.**
* **Classes follow an appropriate inheritance hierarchy.**
* **Derived class functions override virtual base class functions.**
* **The project makes use of references in function declarations.**
* **The project uses smart pointers instead of raw pointers.**
