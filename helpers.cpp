#include "bipartite.h"
#include <climits>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <fstream>
#include <random>
#include <functional>


using namespace std;

static map<int, int> pair_u, pair_v, dist;

extern ofstream fo;


void print_graph(BipartiteGraph G){
    fo<<"Left partition (L): ";

    for(int u : G.L) fo << u << " ";
    fo<<"\nRight partition (R): ";

    for(int u : G.R) fo << u << " ";
    fo<<"\n\nEdges:\n:";


    for(int u = 0; u < G.edge_list.size(); u++){
        for(const auto& e : G.edge_list[u]) {
            if(u < e.to)
                fo<< u << " -- " << e.to << "(cap = " << e.capacity << ")\n";
        }
    }
};

//build level graph for hopcroft karp
static bool bfs(const BipartiteGraph& G){

    queue<int> Q;

    //for every node in left part
    for(int u :  G.L) {

        if(pair_u[u] == -1){
            dist[u] = 0;
            Q.push(u);
        }
        else {
            dist[u] = INT_MAX;
        }

    }

    dist[-1] = INT_MAX;


    while(!Q.empty()){
        
        
        int u = Q.front();
        Q.pop();

        if(dist[u] < dist[-1]){

            for (const Edge& e : G.edge_list[u]) {

                int v = e.to;

                if(pair_v[v] == -1 || (dist[pair_v[v]] == INT_MAX)) {
                    int pv = pair_v[v];
                    dist[pv] = dist[u] + 1;
                    Q.push(pv);
                }


            }

        }

    }

    return dist[-1] != INT_MAX;

};


static bool dfs(int u, const BipartiteGraph& G){


    if(u == -1) return true;


    for(const Edge& e : G.edge_list[u]) {

        int v = e.to;

        if(pair_v[v] == -1 || (dist[pair_v[v]] == dist[u] + 1 && dfs(pair_v[v], G))){
            pair_u[u] = v;
            pair_v[v] = u;
            return true;
        }

    }

    dist[u] = INT_MAX;
    return false;

};


//normal hopcroft karp
double hopcroft_karp(const BipartiteGraph& G){

    pair_u.clear();
    pair_v.clear();
    dist.clear();


    for(int u : G.L) pair_u[u] = -1;
    for (int v : G.R) pair_v[v] = -1;

    int matching = 0;

    while(bfs(G)) {
        for(int u : G.L) {
            if(pair_u[u] == -1 && dfs(u, G)) {
                matching++;
            }
        }
    }

    return matching;
};





//parser for the arguments
void argument_parser(int argc, char* argv[], string &graph_name, string &removal_name, double &epsilon){

    if(argc != 4) {
        cerr << "Usage: " << argv[0] << " <graph-file> <epsilon> <removal-file>\n";
        exit(1);
    }

    graph_name = argv[1];

    try {
        epsilon = stod(argv[2]);
    }
    catch (const exception &e) {
        cerr << "Error: invalid epsilon value " << argv[2] << '\n';
        exit(1);
    }

    removal_name = argv[3];

};

//approx Hopcroft Karp
double HK_approx(BipartiteGraph &G, int L) {
    int n = G.nr_nodes;
    const vector<int> &Lnodes = G.L, &Rnodes = G.R;
    int NL = (int)Lnodes.size(), NR = (int)Rnodes.size();


    vector<int> pairU(n, -1), pairV(n, -1);


    vector<int> dist(n, -1);
    queue<int> Q;

    //BFS that only explores up to depth = 2*L+1
    auto bfs_limited = [&](int max_depth) -> bool {
        fill(dist.begin(), dist.end(), -1);
        // Start from all free U-nodes
        for (int u : Lnodes) {
            if (pairU[u] == -1) {
                dist[u] = 0;
                Q.push(u);
            }
        }
        bool found_augmenting = false;
        while (!Q.empty()) {
            int u = Q.front(); Q.pop();
            if (dist[u] >= max_depth) continue;
            for (auto &e : G.edge_list[u]) {
                if(e.capacity <= 0) continue;
                int v = e.to;
                int pu = pairV[v];
                if (pu == -1) {
                    //we found a free v at depth ≤ max_depth
                    found_augmenting = true;
                } else if (dist[pu] == -1) {
                    dist[pu] = dist[u] + 2;   //u->v->pu adds 2
                    if (dist[pu] <= max_depth)
                        Q.push(pu);
                }
            }
        }
        return found_augmenting;
    };

    //DFS to find augmenting paths within the layering
    function<bool(int,int)> dfs = [&](int u, int max_depth) -> bool {
        for (auto &e : G.edge_list[u]) {
            if(e.capacity <= 0) continue;
            int v = e.to;
            int pu = pairV[v];
            //either v free, or v->pu is next layer
            if ((pu == -1 && dist[u] + 1 <= max_depth) ||
                (pu != -1 && dist[pu] == dist[u] + 2)) {
                //attempt to use this edge
                if (pu == -1 || dfs(pu, max_depth)) {
                    pairU[u] = v;
                    pairV[v] = u;
                    return true;
                }
            }
        }
        dist[u] = -1;  
        return false;
    };

    int matching = 0;
    int max_depth = 2*L + 1;

    for (int phase = 0; phase < L; phase++) {
        if (!bfs_limited(max_depth)) break;
        //for each free u, try to find an augmenting path
        for (int u : Lnodes) {
            if (pairU[u] == -1) {
                if (dfs(u, max_depth)) {
                    matching++;
                }
            }
        }
    }

    return (double)matching;
}

void write_random_bipartite(const std::string &filename, int n, double p) {

    //this function will generate random bipartite graphs for testing
    //i am using p as a probability to include an edge in the graph test
    //moved to separate file


    mt19937_64 rng(random_device{}());

    uniform_real_distribution<double> U(0, 1);

    ofstream fout(filename);

    fout<< (2*n) << '\n';

    for(int u=0; u < n; u++){
        for(int v = n; v < 2*n; v++){
            if(U(rng) < p){
                fout<< u << " " << v << "\n";
            }
        }
    }

    fout.close();

}