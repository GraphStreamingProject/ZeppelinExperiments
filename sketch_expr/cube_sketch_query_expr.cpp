#include <l0_sampling/sketch.h>
#include <l0_sampling/update.h>
#include <stdio.h>

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "../util/stats.h"

const int seed = 0x56FF34FA;

// This test runs the sketching technique introduced by our paper
// and compares it to the one given by AGM
int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Incorrect number of arguments. "
                 "Expected two but got " << argc-1 << std::endl;
    std::cout << "Argument is: vector_length failure_prob" << std::endl;
    exit(EXIT_FAILURE);
  }

  srand(seed);
  const size_t n         = std::atol(argv[1]);
  const size_t fail_prob = std::atol(argv[2]);
  const size_t non_zero  = std::min((size_t)log2(n) * 1000, n / 4);
  const size_t incr      = n / non_zero;
  const size_t num_sktch = 200;
  const size_t loops     = 1000;

  if (fail_prob < 1 || fail_prob > n) {
    std::cerr << "ERROR: failure probability: " << fail_prob << " is out of bounds!" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cerr << "Size of vector = " << n << " delta = " << fail_prob << " non_zero = " << non_zero << std::endl;

  Sketch::configure(n, n);

  Sketch* sketches[num_sktch];
  for (size_t i = 0; i < num_sktch; i++) {
    void* loc = malloc(Sketch::sketchSizeof());
    sketches[i] = Sketch::makeSketch(loc, rand());
  }

  // Update the sketches
  for (vec_t i = 0; i < n; i += incr) {
    for (size_t j = 0; j < num_sktch; j++) {
      sketches[j]->update(i);
    }
  }

  // Query the sketches
  std::vector<double> query_rates;
  double total_query_time = 0;
  std::pair<vec_t, SampleSketchRet> q_ret;
  for (size_t i = 0; i < num_sktch; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t j = 0; j < loops; j++) {
      q_ret = sketches[i]->query();
      if (q_ret.second == GOOD && q_ret.first % incr != 0) std::cout << "BAD INDEX!" << std::endl;

      sketches[i]->reset_queried();
    }
    auto end = std::chrono::high_resolution_clock::now();
    double query_time = std::chrono::duration<double>(end - start).count();
    query_rates.push_back(loops / query_time);
    total_query_time += query_time;
  }
  double dev = stats_std_dev(query_rates);
  double med = stats_median(query_rates);

  // vectorlength, avg_querypersec, median_querypersec, standard_deviation
  std::cout << n << " " << (loops * num_sktch) / total_query_time << " " << med << " " << dev
            << std::endl;

  // cleanup sketches
  for (size_t i = 0; i < num_sktch; i++) {
    free(sketches[i]);
  }
}
