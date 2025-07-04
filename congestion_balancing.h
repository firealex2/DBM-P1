#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "bipartite.h"
#include "helpers.h"

using namespace std;



struct FlowEdge {
    int to, rev;
    double capacity, flow;
};


class FlowNetwork {

public:
    int n;
    vector< vector<FlowEdge> > adj;
    vector <int> level, ptr;
    const double INF = numeric_limits<double>::max();

    FlowNetwork(int nodes);
    
    void add_edge(int u, int v, double cap);

    bool bfs(int s, int t, double delta);

    double dfs(int v, int t, double pushed, double delta);

    double max_flow(int s, int t);



};

FlowNetwork build_flow_network(BipartiteGraph &G);



double matching_or_cut(BipartiteGraph& G, double mu, double epsilon, vector<bool> &SL, vector<bool> &SR, double &current_flow);



bool matching_too_small(BipartiteGraph& G, double& mu, double epsilon);

int robust_matching(BipartiteGraph& G, double& mu, double epsilon, int& doublings);

