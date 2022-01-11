#include "agm_sketching/include/agm_sketch.h"
#include "agm_sketching/include/update.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

// This test runs the sketching technique introduced by our paper
// and compares it to the one given by AGM
int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Incorrect number of arguments. "
                 "Expected one but got " << argc-1 << std::endl;
    std::cout << "Argument is: vector_length" << std::endl;
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  const unsigned long n        = std::atol(argv[1]);
  const unsigned long updates  = 100000; // 100,000 updates
  std::cerr << "Number of nodes = " << n << std::endl;

  Update *stream = new Update[updates];

  // Initialize the stream, and finalize the input vector.
  for (unsigned long i = 0; i < updates; i++) {
    stream[i] = {(long)rand() % n, (rand() % 2) * -2 + 1};
  }

  // cout << "Value of mu: " << mu << std::endl;
  int random     = rand();
  AGM_Sketch sketch  = AGM_Sketch(n, random);

  auto start = std::chrono::steady_clock::now();
  for (unsigned long i = 0; i < updates; i++) {
    sketch.update(stream[i]);
  }
  auto end = std::chrono::steady_clock::now();

  std::chrono::duration<double> ins_time = end - start;
  std::cout << "Time to update a sketch of length " << n 
            << " with " << updates << " updates was: " 
            << ins_time.count() << std::endl;

  std::cout << "Updates per second: " << updates / ins_time.count() << std::endl;
  std::cout << "Sketch is " << sketch.size() << " bytes\n";

  delete[] stream;
}
