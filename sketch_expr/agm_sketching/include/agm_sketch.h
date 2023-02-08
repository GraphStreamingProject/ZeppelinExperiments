#pragma once
#include <cmath>
#include <exception>
#include <iostream>
#include <vector>

#include "bucket.h"
#include "prime_generator.h"
#include "types.h"
#include "update.h"
#include "util.h"

#define bucket_gen(x) double_to_ull(log2(x) + 1)
#define guess_gen(x) double_to_ull(log2(x) + 2)

enum SampleSketchRet {
  GOOD,  // querying this sketch returned a single non-zero value
  ZERO,  // querying this sketch returned that there are no non-zero values
  FAIL   // querying this sketch failed to produce a single non-zero value
};

/**
 * An implementation of a "sketch" as defined in the L0 algorithm.
 * Note a sketch may only be queried once. Attempting to query multiple times
 * will raise an error.
 */
class AGM_Sketch {
  static size_t num_buckets;
  static size_t num_guesses;

  const long seed;
  const vec_t n;
  std::vector<Bucket_Boruvka> buckets;
  const ubucket_t large_prime;
  bool already_quered = false;

  // Initialize a sketch of a vector of size n
 public:
  AGM_Sketch(vec_t n, long seed);

  /* configure the static variables of sketches
   * @param n        Length of the vector to sketch.
   * @param _factor  The rate at which an individual sketch is allowed to fail (determines column width)
   * @return nothing
   */
  inline static void configure(vec_t _n, vec_t _factor) {
    num_buckets = bucket_gen(_factor);
    num_guesses = guess_gen(_n);
  }

  /**
   * Update a sketch based on information about one of its indices.
   * @param update the point update.
   */
  void update(Update update);

  /**
   * Update a sketch given a batch of updates
   * @param begin a ForwardIterator to the first update
   * @param end a ForwardIterator to after the last update
   */
  template <typename ForwardIterator>
  void batch_update(ForwardIterator begin, ForwardIterator end);

  /**
   * Function to query a sketch.
   * @return                        an index in the form of an Update.
   * @throws MultipleQueryException if the sketch has already been queried.
   * @throws NoGoodBucketException  if there are no good buckets to choose an
   *                                index from.
   */
  std::pair<Update, SampleSketchRet> query();

  friend AGM_Sketch operator+(const AGM_Sketch &sketch1, const AGM_Sketch &sketch2);
  friend AGM_Sketch &operator+=(AGM_Sketch &sketch1, const AGM_Sketch &sketch2);
  friend AGM_Sketch operator*(const AGM_Sketch &sketch1, long scaling_factor);
  friend std::ostream &operator<<(std::ostream &os, const AGM_Sketch &sketch);

  // return size of sketch in bytes
  long size() {
    long bucket_size = 2 * sizeof(bucket_t) + sizeof(ubucket_t);
    return sizeof(AGM_Sketch) + bucket_size * bucket_gen(n) * guess_gen(n);
  };

  inline void reset_queried() { already_quered = false; }
};

class AllBucketsZeroException : public std::exception {
 public:
  virtual const char *what() const throw() { return "All buckets zero"; }
};

class MultipleQueryException : public std::exception {
 public:
  virtual const char *what() const throw() {
    return "This sketch has already been sampled!";
  }
};

class NoGoodBucketException : public std::exception {
 public:
  virtual const char *what() const throw() { return "Found no good bucket!"; }
};

template <typename ForwardIterator>
void AGM_Sketch::batch_update(ForwardIterator begin, ForwardIterator end) {
  const unsigned num_buckets = bucket_gen(n);
  const unsigned num_guesses = guess_gen(n);
  for (unsigned i = 0; i < num_buckets; ++i) {
    for (unsigned j = 0; j < num_guesses; ++j) {
      unsigned bucket_id = i * num_guesses + j;
      Bucket_Boruvka &bucket = buckets[bucket_id];
      XXH64_hash_t bucket_seed =
          Bucket_Boruvka::gen_bucket_seed(bucket_id, seed);
      ubucket_t r = Bucket_Boruvka::gen_r(bucket_seed, large_prime);
      std::vector<ubucket_t> r_sq_cache =
          PrimeGenerator::gen_sq_cache(r, n, large_prime);
      for (auto it = begin; it != end; it++) {
        const Update &update = *it;
        if (bucket.contains(update.index, bucket_seed, 1 << j)) {
          bucket.cached_update(update, large_prime, r_sq_cache);
        }
      }
    }
  }
}
