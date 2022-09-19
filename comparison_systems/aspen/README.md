# Aspen

## Aspen Dependencies
GNU C++ compiler version 7
`gcc-7` should be the default compiler for ubuntu18  
Installing gcc-7 is simple on ubuntu20: `sudo apt install gcc-7`  
With ubuntu22, the process is a little more involved:
```
echo "deb [arch=amd64] http://archive.ubuntu.com/ubuntu focal main universe" | sudo tee -a /etc/apt/sources.list
sudo apt update
sudo apt install gcc-7
```

`jemalloc` malloc implementation

```
sudo apt install libjemalloc-dev
```

You are done setting up Aspen, return to [README.md](README.md). Supplementary information is below.

## Aspen Experiments

### Ingestion Test
The command line arguments to the `ingestion_test` executable are as follows:

1. Path to the (binary) stream file
2. Update batch size. Unlike our system, Aspen is not designed to process updates at the granularity of a single edge. Instead,
   Aspen only accepts updates in batches of a single type (i.e. either all edge insertions or all edge deletions). This parameter controls how
   large this batch should be. We used a batch size of 1,000,000 for the results we reported in the paper.
3. Buffer size. This parameter controls how many edges we should retrieve from external memory at once. We used a buffer size of
   1,000,000 for the results we reported in the paper.
4. Path to file to write timing data.

If you'd like to take a closer look at the actual source code of the
speed and memory consumtion test, you may find it in
`code/tools/ingestion_test.cpp`.


### Continous Query Test
The command line arguments to the `continuous_query_test` executable are as follows:

1. Path to the (binary) stream file
2. Update batch size. Unlike our system, Aspen is not designed to process updates at the granularity of a single edge. Instead,
   Aspen only accepts updates in batches of a single type (i.e. either all edge insertions or all edge deletions). This parameter controls how
   large this batch should be. We used a batch size of 1,000,000 for the results we reported in the paper.
3. Buffer size. This parameter controls how many edges we should retrieve from external memory at once. We used a buffer size of
   1,000,000 for the results we reported in the paper.
4. Path to file where we record the results of continuous query testing.
5. Number of CC queries. If this parameter is X, then we perform X connected components queries every 1/X of the way through the stream.

If you'd like to take a closer look at the actual source code of the
continuous query test, you may find it in 
`code/tools/continuous_query_test.cpp`

