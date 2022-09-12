# Terrace
## Building Terrace
### System Requirements
Operating System: Ubuntu18.04, Ubuntu20.04
### Dependencies
#### Packages
`sudo apt install libssl-dev libz3-dev`
#### OpenCilk
Terrace uses the OpenCilk compiler v1.0. To install OpenCilk:
##### Ubuntu-20.04
```
wget https://github.com/OpenCilk/opencilk-project/releases/download/opencilk%2Fv1.0/OpenCilk-1.0-LLVM-10.0.1-Ubuntu-20.04-x86_64.sh
sudo mkdir /opt/opencilk
sudo sh OpenCilk-1.0-LLVM-10.0.1-Ubuntu-20.04-x86_64.sh --prefix=/opt/opencilk --exclude-subdir
```
##### Ubuntu-18.04
```
wget https://github.com/OpenCilk/opencilk-project/releases/download/opencilk%2Fv1.0/OpenCilk-1.0-LLVM-10.0.1-Ubuntu-18.04-x86_64.sh
sudo mkdir /opt/opencilk
sudo sh OpenCilk-1.0-LLVM-10.0.1-Ubuntu-18.04-x86_64.sh --prefix=/opt/opencilk --exclude-subdir
```

### Build Command
`(export CILK=1 ; export D=0 ; make -j ingestion_test continuous_query_test)`

## Running Experiments
### Ingestion Experiment
The command line arguments to the `ingestion_test` executable are as follows:

1. Path to the (binary) stream file
2. Update batch size. Unlike our system, Terrace is not designed to process updates at the granularity of a single edge. Instead,
   Terrace only accepts updates in batches of a single type (i.e. either all edge insertions or all edge deletions). This parameter controls how
   large this batch should be. We used a batch size of 1,000,000 for the results we reported in the paper.
3. Buffer size. This parameter controls how many edges we should retrieve from external memory at once. We used a buffer size of
   1,000,000 for the results we reported in the paper.
4. Path to file to write timing data.

If you'd like to take a closer look at the actual source code of the
speed and memory consumtion test, you may find it in
`code/src/ingestion_test.cpp`.

### Continuous Query Experiment
The command line arguments to the `cont_query` executable are as follows:

1. Path to the (binary) stream file
2. Update batch size. Unlike our system, Terrace is not designed to process updates at the granularity of a single edge. Instead,
   Terrace only accepts updates in batches of a single type (i.e. either all edge insertions or all edge deletions). This parameter controls how
   large this batch should be. We used a batch size of 1,000,000 for the results we reported in the paper.
3. Buffer size. This parameter controls how many edges we should retrieve from external memory at once. We used a buffer size of
   1,000,000 for the results we reported in the paper.
4. Path to file where we record the results of continuous query testing.
5. Number of CC queries. If this parameter is X, then we perform X connected components queries every 1/X of the way through the stream.

If you'd like to take a closer look at the actual source code of the
continuous query test, you may find it in 
`code/src/continuous_query_test.cpp`.

