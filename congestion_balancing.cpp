#include <climits>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

#include "bipartite.h"
#include "helpers.h"
#include "congestion_balancing.h"

using namespace std;

extern ofstream fo;





FlowNetwork::FlowNetwork(int nodes){
    adj.resize(nodes);
    n = nodes;
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
                if(level[e.to] == -1 && e.capacity - e.flow > 1e-9) {
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

    for(int &id = ptr[v]; id < (int)adj[v].size(); id++){

        FlowEdge &e = adj[v][id];

        if(level[v] + 1 != level[e.to] || e.capacity - e.flow < 1e-9) continue;

        double tr = dfs(e.to, t, min(pushed, e.capacity - e.flow), 0);

        if(tr > 0){
            e.flow += tr;
            adj[e.to][e.rev].flow -= tr;
            return tr;
        }

    }

    return 0;

}

double FlowNetwork::max_flow(int s, int t){
    double flow = 0;
    double max_capacity = 0;

    //find max capacity
    for (int i = 0; i < n; i++){
        for(auto &e : adj[i]){
            max_capacity = max(max_capacity, e.capacity);
        }
    }

    //for (double delta = pow(2, floor(log2(max_capacity))); delta >= 1e-9; delta/=2){
        
        while(bfs(s, t, 0)) {
            ptr.assign(n, 0);
            while (double pushed = dfs(s, t, INF, 0)){
                flow += pushed;
            }
        }

    //}

    return flow;
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


double matching_or_cut(BipartiteGraph& G, double mu, double epsilon, vector<bool> &SL, vector<bool> &SR, double &current_flow){


    fo<<"IN MATCHING OR CUT: \n"<<"-------------\n";

    int n = G.nr_nodes;
    int s = n;
    int t = n + 1;

    //make flow network out of the graph
    FlowNetwork F = build_flow_network(G);
    F.n = n + 2;


    //double threshold = G.L.size() * (1 - epsilon);
    double threshold = mu * (1 - 3 * epsilon);
    double flow = F.max_flow(s, t);

    //check flow (fractional matching size)

    fo<<"FLOW CALCULATED IS: "<<flow<<" ----------- "<<endl;
    fo<<"THRESHOLD IS: "<< threshold<<" ----------- "<<endl;

    current_flow = flow;

    if(flow >= threshold) {
        //it is good

        return flow;
    }



    vector<bool> reachable(F.n, false);
    fo<<"Fn = "<<F.n <<endl;

    queue<int> Q;
    Q.push(s);
    reachable[s] = true;

    


    while(!Q.empty()) {
        int u = Q.front();
        Q.pop();


        for(FlowEdge& e : F.adj[u]){
            
            if(!reachable[e.to] && (e.capacity - e.flow) > 1e-9) {
                reachable[e.to] = true;
                Q.push(e.to);
            }

        }

    }

    fill(SL.begin(), SL.end(), false);
    fill(SR.begin(), SR.end(), false);


    fo<<"SL creation: \n";
    for(int u : G.L){
        
        if(reachable[u]){
            fo<<u << " ";
            SL[u] = true;
        }
    }
    fo<<"\nSR creation: \n";
    for(int u : G.R){
        
        if(reachable[u]){
            fo<<u<<" ";
            SR[u] = true;
        }
    }

    fo<<"\n";
    return -1;


}

bool matching_too_small(BipartiteGraph& G, double& mu, double epsilon){

    fo<<"IN MATCHING TOO SMALL: \n"<<"-------------\n";

    double aux = mu;
    mu = hopcroft_karp(G);

    //print_graph(G);

    fo<<"CALCULATED MU IS: "<<mu<<endl;

    return (mu < aux*(1 - 2*epsilon)) ? true : false; 

}

void robust_matching(BipartiteGraph& G, double& mu, double epsilon){

    fo<<"IN ROBUST MATCHING TOO SMALL: \n"<<"-------------\n";


    // we need the size of L to ne a power of 2.
    // otherwise replace  with n = 2^[log(n)]

    //init graph 


    //init the k(e) = 1/n for each edge


    //begin new phase

    //if matching too small we return
    if(matching_too_small(G, mu, epsilon)){
        fo<<"Matching too small\n"<<endl;
        return;
    }




    //initialize the cuts as bool 
    vector <bool> SL (G.nr_nodes, false);
    vector <bool> SR (G.nr_nodes, false);



    double flow = 0;
    double prev_flow = -1;
    double current_flow = 0;
    //while we do not find a matching
    while (true) {

        fo<<"NEW MU is: "<<mu<<endl;

        bool doubled_any = false;

        //work on the sets
        //find the edges from SL to R\SR
        //cout<<"Current threshold for matching: "<<mu * (1 - 4 * epsilon)<<endl;


        double flow = matching_or_cut(G, mu, epsilon, SL, SR, current_flow);

        cout<<prev_flow << " " << current_flow << endl;

        if(flow >= 0 ){
            cout<<"Exited: found good flow\n";
            return;
        }

        if(prev_flow == current_flow){
            cout<<"Exited: hard cap on doubling\n";
            return;
        }

        prev_flow = current_flow;

        fo<<"flow is: "<<flow<<endl;


        for (int u : G.L) {

            if(!SL[u]) continue;
            for(auto &e : G.edge_list[u]) {

                int v = e.to;

                if(find(G.R.begin(), G.R.end(), v) != G.R.end() && !SR[v]) {
                    e.capacity *= 2.0;
                    doubled_any = true;
                }

            }
        }

        if(!doubled_any){
            return;
        }

    }



    return;




}

