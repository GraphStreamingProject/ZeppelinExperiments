# ZeppelinExperiments
Experiments used in Graph Zeppelin paper appearing in SIGMOD'22.

## System Requirements
### GraphZeppelin and Experiments
* OS: Linux (tested on ubuntu-20 and ubuntu-18)
* RAM: 64GB
* Disk (SATA): 2 SSDs minimum 0.5 TB (we used SAMSUNG 870 EVO (1TB)) (one holding data can be somewhat slow)
* CPU: we used: Intel(R) Xeon(R) Gold 5220R CPU @ 2.20GHz (24 cores hyper threaded to 48)
* C++ 14
* cmake version 3.15 or higher

Example EC2 instances: c5d.9xlarge or c5d.12xlarge

## Installation
### 1. General Packages
```
sudo apt update
sudo apt install build-essential git r-base cgroup-tools libboost-all-dev texlive-latex-base texlive-pictures texlive-science libxml2-dev libcurl4-openssl-dev
```

### 2. Install cmake version 3.15+
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

### 3. Setup cgroups
We use `cgroups` to limit the amount of memory available to GraphZeppelin, Apsen, or Terrace. A Control Group is a linux kernel feature. The following steps create `cgroups` for limiting memory to 16 GiB and 8 GiB.

```
cd /sys/fs/cgroup/memory
sudo mkdir 16_GB 12_GB 8_GB
sudo chown -R {YOUR USERNAME} 16_GB 12_GB 8_GB
cd 16_GB
echo 1 > memory.oom_control
echo 16G > memory.limit_in_bytes
echo 16G > memory.soft_limit_in_bytes
cd ../12_GB
echo 1 > memory.oom_control
echo 12G > memory.limit_in_bytes
echo 12G > memory.soft_limit_in_bytes
cd ../8_GB
echo 1 > memory.oom_control
echo 8G > memory.limit_in_bytes
echo 8G > memory.soft_limit_in_bytes
```

### 4. Create a swapfile
We placed our swapfile on the same disk that held GraphZeppelin's on-disk data-structures. You can create a swapfile as follows:

First define where the swapfile will be located.
`export SWAP_LOC=/path/to/swapfile`

Then copy and paste the following:
```
sudo fallocate -l 150G $SWAP_LOC
sudo chmod 600 $SWAP_LOC
sudo mkswap $SWAP_LOC
sudo swapon $SWAP_LOC
```
### 5. Clone repo and install R dependencies
R dependencies:
```
sudo Rscript plotting/R_scripts/install.R
```

### 5. Other systems
Follow the dependency/installation instructions for [Aspen](comparison_systems/aspen/README.md) and [Terrace](comparison_systems/terrace/README.md)

## Running Experiments
Once the Installation steps have been completed, experiments can be run with `./run.sh`.

Be ready to supply two on disk locations for data storage (datasets and GraphZeppelin's own data-structures). Ideally these two locations will be on seperate disks.

### Experiment Options
We give two options for how to run our experiments: `limited` or `full`. The limited experiments run fewer iterations of some of our experiments and use more aggressive timeouts. The full experiments run all experiments used in our paper with the 24hr timeouts we used.

Limited experiments runtime: roughly 48 hours.  
Full experiments runtime: roughly 4-5 days.
