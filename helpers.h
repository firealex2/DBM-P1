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
