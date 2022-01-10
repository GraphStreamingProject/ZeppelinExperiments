#include <l0_sampling/sketch.h>
#include <l0_sampling/update.h>

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
  const long n        = std::atol(argv[1]);
  const long updates  = 100000; // 100,000 updates

  vec_t *stream = new vec_t[updates];

  // Initialize the stream, and finalize the input vector.
  for (unsigned int i = 0; i < updates; i++) {
    stream[i] = (long)rand() % n;
  }

  // cout << "Value of mu: " << mu << std::endl;
  int random     = rand();
  Sketch::configure(n, n);
  void *loc = malloc(Sketch::sketchSizeof());
  Sketch *sketch = Sketch::makeSketch(loc, random);

  auto start = std::chrono::steady_clock::now();
  for (unsigned int i = 0; i < updates; i++) {
    sketch->update(stream[i]);
  }
  auto end = std::chrono::steady_clock::now();

  std::chrono::duration<double> ins_time = end - start;
  std::cout << "Time to update a sketch of length " << n 
            << " with " << updates << " updates was: " 
            << ins_time.count() << std::endl;

  std::cout << "Updates per second: " << updates / ins_time.count() << std::endl;
  std::cout << "Sketch is " << Sketch::sketchSizeof() << " bytes\n";

  delete[] stream;
}
