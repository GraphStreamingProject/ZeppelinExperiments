#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <vector>
#include "types.h"

namespace PrimeGenerator{
  inline ubucket_t powermod(ubucket_t a, ubucket_t b, ubucket_t m) {
    ubucket_t res = 1;
    while (b) {
      if (b & 1) {
        res = static_cast<ubucket_t>(static_cast<ubucket_prod_t>(res) * a % m);
      }
      b >>= 1;
      a = static_cast<ubucket_t>(static_cast<ubucket_prod_t>(a) * a % m);
    }
    return res;
  }

  //Generate a^2^i for i in [0, log_2(n)]
  inline std::vector<ubucket_t> gen_sq_cache(ubucket_t a, ubucket_t n, ubucket_t m) {
    unsigned log2n = log2(n);
    std::vector<ubucket_t> sq_cache(log2n + 1);
    sq_cache[0] = a;
    for (unsigned i = 0; i < log2n; i++) {
      sq_cache[i + 1] = static_cast<ubucket_t>(static_cast<ubucket_prod_t>(sq_cache[i]) * sq_cache[i] % m);
    }
    return sq_cache;
  }

  inline ubucket_t cached_powermod(const std::vector<ubucket_t>& sq_cache, ubucket_t b, ubucket_t m) {
    unsigned i = 0;
    ubucket_t res = 1;
    while (b) {
      if (b & 1) {
        res = static_cast<ubucket_t>(static_cast<ubucket_prod_t>(res) * sq_cache[i] % m);
      }
      b >>= 1;
      i++;
    }
    return res;
  }

  // prime testing using witness testing
  // the witnesses chosen here work for all numbers < 2^64
  // based upon a wikipedia article on the Miller Rabin test
  static bool IsPrime(ubucket_t n) {
    if (n == 2) return true;
    if (n < 2 || n % 2 == 0) return false;

    // Convert n to form (2^m * d) + 1
    ubucket_t d = n - 1;
    int m = 0;
    while (d % 2 != 0) {
      d = d >> 1;
      m += 1;
    }

    // Check each witness (m times)
    ubucket_t witnesses[12] = {2,3,5,7,11,13,17,19,23,29,31,37};
    for (int i = 0; i < 12; i++) {
      ubucket_t witness = witnesses[i];
      if (witness >= n) return true;

      ubucket_t result = powermod(witness, d, n);

      if (result == 1 || result == n - 1)
        continue;

      bool approve = false;
      ubucket_t temp = witness;
      for (int j = 1; j < m; j++) {
        temp *= temp;
        result = powermod(temp, d, n);
        if (result == 1 || result == n - 1) {
          approve = true;
          break;
        }
      }
      if (!approve) {
        return false;
      }
    }

    return true;
  }

  //Generates a prime number greater than or equal to n
  inline ubucket_t generate_prime(ubucket_t n){
    if (n % 2 == 0){
      n++;
    }
    while(!IsPrime(n)){
      n += 2;
    }
    return n;
  }
}