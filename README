# Aspen Experiment Setup
Prior to building any executables, make sure the CILK flag environment
variable is defined by running something like "export CILK=1" in the
console you'll be building from.

## Setting up Aspen (from the Aspen README)

### Hardware Dependencies

Any modern x86-based multicore machine can be used. Aspen uses 128-bit CMPXCHG
(requires -mcx16 compiler flag) but does not need hardware transactional memory
(TSX). Most (small graph) examples in our artifact can be run using fewer than
100GB of memory, but our tests on larger graph (ClueWeb and larger) require
200GB--1TB of memory. All examples below can run in 100GB of memory by using a
small graph input.

### Software Dependencies
Aspen requires g++ 5.4.0 or later versions supporting the Cilk Plus extensions.
The scripts that we provide in the repository use `numactl -i all` for better
performance. However, all tests can also run directly without `numactl`.

### Others
In addition to the above Aspen dependencies, we also need `g++ 7` to make our executables

## Speed and Memory Test

To build the executable for the speed and memory consumption test, run 
`make ingestion_test`

The command line arguments to the resulting `ingestion_test` executable are as
follows:

1. Path to the (binary) stream file
2. Update batch size. Unlike our system, Aspen is not designed to process updates at the granularity of a single edge. Instead,
   Aspen only accepts updates in batches of a single type (i.e. either all edge insertions or all edge deletions). This parameter controls how
   large this batch should be. We used a batch size of 1,000,000 for the results we reported in the paper.
3. Buffer size. This parameter controls how many edges we should retrieve from external memory at once. We used a buffer size of
   1,000,000 for the results we reported in the paper.
4. Path to file to write timing data.

If you'd like to take a closer look at the actual source code of the
speed and memory consumtion test, you may find it in
`./code/tools/ingestion_test.cpp`.

## Continuous Query Testing
To build the executable for the continuous query testing experiment, run 
`make continuous_query_test`

The command line arguments to the resulting `cont_query` executable are as
follows:

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
`./code/tools/continuous_query_test.cpp`.
