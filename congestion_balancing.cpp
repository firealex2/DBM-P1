#include <climits>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "bipartite.h"
#include "helpers.h"
#include "congestion_balancing.h"

using namespace std;





FlowNetwork::FlowNetwork(int nodes){
    adj.resize(nodes);
}
    
void FlowNetwork::add_edge(int u, int v, double cap){

    FlowEdge a = {v, (int)adj[v].size(), cap, 0};
    FlowEdge b = {u, (int)adj[u].size(), 0, 0}; //reverse edge

    adj[u].push_back(a);
    adj[v].push_back(b);


}

bool FlowNetwork::bfs(int s, int t, double delta){

        level.assign(n, -1);


        queue<int> Q;
        level[s] = 0;
        Q.push(s);


        while(!Q.empty()) {
            
            int v = Q.front();
            Q.pop();

            for(FlowEdge &e : adj[v]){
                if(level[e.to] == -1 && e.capacity - e.flow >= delta) {
                    level[e.to] = level[v] + 1;
                    Q.push(e.to);
                }
            }

        }

    return level[t] != -1;

} 

double FlowNetwork::dfs(int v, int t, double pushed, double delta){

    if (pushed == 0) return 0;

    if (v == t) return pushed;

}

double FlowNetwork::max_flow(int s, int t){

}


FlowNetwork build_flow_network(BipartiteGraph &G){

    int n = G.nodes.size();

    //set the source and sink as the last two nodes of the graph
    int source = n;
    int sink = n + 1;

    FlowNetwork network(n+2);

    //add an edge from source to everybody in L + all the edges in G


    for(int u : G.L) {
        
        network.add_edge(source, u, 1.0);

        //for all the edges that go from u
        for (Edge &e : G.edge_list[u]) {
            network.add_edge(u, e.to, e.capacity);
        }
    }

    //add an edge from everybody in R to sink

    for(int v : G.R) {
        network.add_edge(v, sink, 1.0);
    }


    return network;

}


int matching_or_cut(BipartiteGraph& G, int mu, double epsilon, map<Node, int>& SL, map<Node, int>& SR){

}

bool matching_too_small(BipartiteGraph& G, int mu, double epsilon){

    int m = hopcroft_karp(G);

    return (m < mu*(1 - 2*epsilon)) ? true : false; 

}

void robust_matching(BipartiteGraph& G, int mu, double epsilon){

    // we need the size of L to ne a power of 2.
    // otherwise replace  with n = 2^[log(n)]

    //init graph 


    //init the k(e) = 1/n for each edge

    int n = G.nr_nodes;

    for(int i = 0; i < n; i++){
            
        for(Edge& e : G.edge_list[i]){
            e.capacity = 1.0 / n;
        }    

    }

    //begin new phase

    //if matching too small we return
    if(matching_too_small(G, mu, epsilon)){
        return;
    }

    // vector<Node> SL, SR;

    // SL.clear();
    // SR.clear();

    map<Node, int> SL, SR;

    SL.clear();
    SR.clear();


    //while we do not find a matching
    while (matching_or_cut(G, mu, epsilon, SL, SR) == INT_MAX) {

        //work on the sets
        //find the edges from SL to R\SR

        for(const auto& [node, value] : SL){
            
            //go through edges of each node

            for(const auto& [to, position] : node.incident_edges){

                //if it is not in the map it means it is on the right side R\SR
                if(SR[G.nodes[to]] != 1){

                    G.edge_list[node.id][position].capacity = 2 *  G.edge_list[node.id][position].capacity;

                }

            }

        }

        SL.clear();
        SR.clear();
    }





}

