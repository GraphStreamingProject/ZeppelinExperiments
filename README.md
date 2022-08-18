# ZeppelinExperiments
Experiments used in Graph Zeppelin paper. Paper submitted to SIGMOD 2022.

# TODOs
* How do we provide our kron graphs for reviewers to run? -- automated get and place in right spot on SSD
* Aspen and Terrace code and build instructions -- can we have cmake do this? Other installation scripts
* ~Expand installation instructions a little bit to include installing cmake etc.~
* ~Setup instructions for cgroup~
* One big experiment bash file (robust and front-load quick experiments)
* Format output of experiments to be in nice CSV format for R scripts
* ~Add R scripts code to repo and note dependencies~
* Modifying top configuration -- Check Victor's email for instructions.

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
### 1. Install cmake version 3.15+
First Step:
#### x86_64
```
wget https://github.com/Kitware/CMake/releases/download/v3.23.0-rc2/cmake-3.23.0-rc2-linux-x86_64.sh
sudo mkdir /opt/cmake
sudo sh cmake-3.23.0-rc2-linux-x86_64.sh --prefix=/opt/cmake
```
#### aarch64
```
wget https://github.com/Kitware/CMake/releases/download/v3.23.0-rc5/cmake-3.23.0-rc5-linux-aarch64.sh
sudo mkdir /opt/cmake
sudo sh cmake-3.23.0-rc5-linux-aarch64.sh --prefix=/opt/cmake
```
Second Step:
```
sudo ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
```
When running cmake .sh script enter y to license and n to install location.  
These commands install cmake version 3.23 but any version >= 3.15 will work.

### Setup cgroups
We use `cgroups` to limit the amount of memory available to GraphZeppelin, Apsen, or Terrace. A Control Group is a linux kernel feature. The following steps create `cgroups` for limiting memory to 16 GiB and 8 GiB.

```
cd /sys/fs/cgroup/memory
sudo mkdir 16GB ; sudo mkdir 8GB
chown -R {USERNAME} 16GB ; chown -R {USERNAME} 8GB
cd 16GB
echo 1 > memory.oom_control
echo 16G > memory.limit_in_bytes
echo 16G > memory.soft_limit_in_bytes
cd ../8GB
echo 1 > memory.oom_control
echo 8G > memory.limit_in_bytes
echo 8G > memory.soft_limit_in_bytes
```

### Installing Experiments Repository
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

