#include <vector>
#include <cmath>
#include <algorithm>

inline double stats_sum(const std::vector<double>& vec) {
  double sum = 0;
  for (double elm : vec) {
    sum += elm;
  }
  return sum;
}

inline double stats_mean(const std::vector<double>& vec) {
  return stats_sum(vec) / vec.size();
}

inline double stats_median(const std::vector<double>& vec) {
  std::vector<double> sorted(vec);
  std::sort(sorted.begin(), sorted.end());
  return sorted[vec.size() / 2];
}

inline double stats_std_dev(const std::vector<double>& vec) {
  double stdev = 0;
  double m = stats_mean(vec);
  for (double elm : vec)
    stdev += (elm - m) * (elm - m);

  return sqrt(stdev / vec.size());
}
