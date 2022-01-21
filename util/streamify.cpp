#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <queue>

using namespace std;

const int notification_frequency = 1e6;
const unsigned max_ccs = 4096; // 2^12
const int num_cc_combinations = 12;
unsigned long long total_edges = 4294967296;

double static_reinsertion_param = 0.5;
unsigned inv_static_reinsertion_param = 2;

const double max_uniform_generator_range = 40000000.0;

const string temp_helper_file = "./TEMP_HELPER";

/** ASSUMPTIONS
 * 1. only up to 2^32 nodes
 * 2. Input graph is undirected - i.e. if an edge (u, v) appears in the
 *    graph, then (v, u) doesn't.
 * 3. Input graph is simple.
 * 4. Input graph contains edges in a random order (i.e. not sorted)
 */

int main (int argc, char * argv []) {
  if (argc != 4) {
    cout << "Streamifier usage\n\t"
            "streamify static_graph_filepath num_nodes stream_filepath" << endl;
    return 1;
  }
  string static_graph_file_name = argv[1];
  unsigned int num_nodes;
  sscanf(argv[2], "%u", &num_nodes);
  string stream_file_name = argv[3];

  ifstream static_graph_file{static_graph_file_name};
  if (!static_graph_file.is_open()) {
    std::cout << "Couldn't open input graph file!" << std::endl;
    return 1;
  }


  typedef std::pair<unsigned int, unsigned int> edge;
  vector<edge> updates;
  // TODO: reserve some appropriate space to minimize resizing

  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  mt19937_64 generator(seed);

  unsigned long total_num_updates = 0;

  // Allocate a bitvector array with one entry per possible edge

  cout << "Allocating bit vectors" << endl;
  vector<vector<bool>> edge_present(num_nodes);
  for (unsigned int i = 0; i < num_nodes - 1; i++)
    edge_present[i] = vector<bool>(num_nodes - 1 - i, false);

  // Create different numbers of CCs as a function of time through the stream,
  // changing mod every notification_frequency edges read

  vector<unsigned> cc_mod;
  unsigned long long num_rounds = total_edges / notification_frequency / num_cc_combinations;
  int cnt = 0;
  for (unsigned i = 0; i <= total_edges / notification_frequency; ++i) {
    unsigned num_ccs = max(max_ccs >> cnt, 1u);
    for (unsigned j = 0; j < num_rounds; ++j, ++i) {
      cc_mod.push_back(num_ccs);
    }
    ++cnt;
  }
  sort(cc_mod.begin(), cc_mod.end(), greater<unsigned>());
  unsigned mod_ptr = 0;

  // Static graph geometric inserts/deletes

  cout << "Constructing updates for edges in input graph..." << endl;

  ofstream temp_out {temp_helper_file};
  priority_queue<pair<double,edge>> insertion_pq;
  queue<edge> good_edges;
  uniform_real_distribution<double> uniform_dist {0.0, 4000000000.0};
  unsigned int u, v;
  unsigned long k = 0;
  while (static_graph_file >> u >> v) {
    if (k % notification_frequency == 0 && k != 0) {
      cout << k << " edges completed..." << endl;

      cout << "Good edges: " << good_edges.size() << endl;
      // custom geometric generator to create roughly notification_frequency edge updates
      double param = min((double) good_edges.size() / notification_frequency,
                         static_reinsertion_param);
      cout << "geometric param: " << param << endl;
      geometric_distribution<unsigned long> static_reinsertions(param);

      // add all edges to the stream
      while (!good_edges.empty()) {
        auto ee = good_edges.front();
        good_edges.pop();
        auto num_repetitions = static_reinsertions(generator);
        for (unsigned long j = 0; j < num_repetitions; j++) {
          insertion_pq.push({uniform_dist(generator), ee});
        }
      }

      // flush to temp output
      total_num_updates += insertion_pq.size();
      char upd_type;
      while (!insertion_pq.empty()) {
        auto ee = insertion_pq.top().second;
        u = ee.first;
        v = ee.second;
        if (u > v) swap(u,v);
        insertion_pq.pop();
        upd_type = edge_present[u][v - u - 1] ? '1' : '0';
        edge_present[u][v - u - 1] = !edge_present[u][v - u - 1];

        temp_out << upd_type << '\t' << u << '\t' << v << '\n';
      }

      ++mod_ptr;
    }

    // if we can, add to set of good edges
    if ((v - u) % cc_mod[mod_ptr] == 0) {
      good_edges.push({u,v});
    }
    k++;
  }

  // finish with all edges
  // custom geometric generator to create roughly notification_frequency edge updates
  geometric_distribution<unsigned long> static_reinsertions(static_reinsertion_param);

  // add all edges to the stream
  while (!good_edges.empty()) {
    auto ee = good_edges.front();
    good_edges.pop();
    auto num_repetitions = static_reinsertions(generator);
    for (unsigned long j = 0; j < num_repetitions; j++) {
      insertion_pq.push({uniform_dist(generator), ee});
    }
  }

  // flush to temp output
  total_num_updates += insertion_pq.size();
  char upd_type;
  while (!insertion_pq.empty()) {
    auto ee = insertion_pq.top().second;
    u = ee.first;
    v = ee.second;
    if (u > v) swap(u,v);
    insertion_pq.pop();
    upd_type = edge_present[u][v - u - 1] ? '1' : '0';
    edge_present[u][v - u - 1] = !edge_present[u][v - u - 1];

    temp_out << upd_type << '\t' << u << '\t' << v << '\n';
  }
  temp_out.close();

//  // In-memory shuffle of updates
//
//  cout << "Conducting in-memory shuffle of generated updates..." << endl;
//  std::shuffle(updates.begin(), updates.end(), generator);

  // Output updates to stream file and add update types

  cout << "Writing stream to file..." << endl;

  ofstream stream_file_out{stream_file_name};
  ifstream temp_in {temp_helper_file};
  if (!stream_file_out.is_open())
  {
    std::cout << "Couldn't open output stream file!" << std::endl;
    return 1;
  }

  stream_file_out << num_nodes << ' ' << total_num_updates << '\n';

  string temp_string;
  while (getline(temp_in, temp_string))
    stream_file_out << temp_string << "\n";

  return 0;
}