// helper functions
#pragma once
#include <string>
#include "bipartite.h"

/* here we are going to have: 

    - print functions
    - max flow algorithm
    - extract cut
    - timing
    - hopcroft karp

*/
using namespace std;



static bool bfs(const BipartiteGraph& G);
static bool dfs(int u, const BipartiteGraph& G);

void print_graph(BipartiteGraph g);


/*
Use BFS to build tree
Use DFS to find augumenting paths
Stop when no more augmenting paths exist

@param g Graph that we are going to urn Hopcroft Karp on


returns the size of the matching
*/
double hopcroft_karp(const BipartiteGraph& G);
double HK_approx(BipartiteGraph &G, int L);

double max_flow(BipartiteGraph g);

void argument_parser(int argc, char* argv[], std::string &graph_name, std::string &removal_name, double &epsilon);


void write_random_bipartite(const std::string &filename, int n, double p);

// Logging helper functions
void log_separator(const std::string& title = "");
void log_graph_state(const BipartiteGraph& G, const std::string& context = "");
void log_phase_start(int phase_num, double mu, double epsilon, double accumulated_weight);
void log_matching_or_cut_result(double flow, double threshold, bool found_matching);
void log_cut_info(const BipartiteGraph& G, const std::vector<bool>& SL, const std::vector<bool>& SR);
void log_doublings(int total_doublings, int this_iteration);
