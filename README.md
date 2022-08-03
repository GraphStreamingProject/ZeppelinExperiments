# ZeppelinExperiments
Experiments used in Graph Zeppelin paper. Paper submitted to SIGMOD 2022.

# TODOs
* How do we provide our kron graphs for reviewers to run? -- automated get and place in right spot on SSD
* Aspen and Terrace code and build instructions -- can we have cmake do this? Other installation scripts
* Expand installation instructions a little bit to include installing cmake etc.
* Setup instructions for cgroup
* One big experiment bash file (robust and front-load quick experiments)
* Format output of experiments to be in nice CSV format for R scripts
* Add R scripts code to repo and note dependencies

## System Requirements
### GraphZeppelin and Experiments
* OS: Linux (tested on ubuntu-20 and ubuntu-18)
* RAM: 64GB
* Disk (SATA): 2 SSDs minimum 0.5 TB (we used SAMSUNG 870 EVO (1TB)) (one holding data can be somewhat slow)
* CPU: we used: Intel(R) Xeon(R) Gold 5220R CPU @ 2.20GHz (24 cores hyper threaded to 48)
* C++ 14
* cmake version 3.15 or higher

### Additional requirements for comparison systems
Aspen
* CILK for Aspen?

Terrace
* ?

Example EC2 instances: c5d.9xlarge or c5d.12xlarge

## Installation
1. Clone this repository
2. Create a `build` directory in the directory where you cloned this repository.
3. Initialize cmake by running `cmake ..` in the build dir.
4. Build the experiments by running `cmake --build .` in the build dir.
5. (somehow specify the storage disk and the swap+guttertree disk)
6. (Run some script to download data and do setup)

## Running Experiments
(Run the global experiments bash script)
### Speed and Size Experiment

### Parallel Experiment

### Buffer Size Experiment

### Correctness Experiment

### Sketch Speed and Size Experiment

