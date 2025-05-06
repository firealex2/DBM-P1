#include "bipartite.h"
#include <climits>
#include <iostream>
#include <map>
#include <queue>

using namespace std;

static map<int, int> pair_u, pair_v, dist;


void print_graph(BipartiteGraph G){
    cout<<"Left partition (L): ";

    for(int u : G.L) cout << u << " ";
    cout<<"\nRight partition (R): ";

    for(int u : G.R) cout << u << " ";
    cout<<"\n\nEdges:\n:";


    for(int u = 0; u < G.edge_list.size(); u++){
        for(const auto& e : G.edge_list[u]) {
            if(u < e.to)
                cout<< u << " -- " << e.to << "(cap = " << e.capacity << ")\n";
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