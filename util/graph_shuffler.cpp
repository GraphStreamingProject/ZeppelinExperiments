#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>

const std::string in_file = "/mnt/ssd2/new_graphs/kron_17_half_no_dup.txt";
const std::string out_file =
      "/mnt/ssd2/new_graphs/kron_17_SHUFFLED.txt";
int main() {
  std::ifstream in {in_file};
  unsigned long long n,m;
  in >> n >> m;
  std::string temp;

  std::vector<std::pair<uint32_t, uint32_t>> edges {m};
  for (uint64_t i = 0; i < m; ++i) {
    in >> edges[i].first >> edges[i].second;
  }
  in.close();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937_64 generator {seed};

  std::cout << "Beginning shuffle..." << std::endl;
  std::shuffle(edges.begin(), edges.end(), generator);
  std::cout << "Shuffle done" << std::endl;

  std::ofstream out {out_file};
  out << n << " " << m << "\n";
  for (uint64_t i = 0; i < edges.size(); ++i) {
    out << edges[i].first << "\t" << edges[i].second << "\n";
  }
  out.close();
  return 0;
}