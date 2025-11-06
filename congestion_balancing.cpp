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
    max_capacity = 0.0;
}
    
void FlowNetwork::add_edge(int u, int v, double cap){

    FlowEdge a = {v, (int)adj[v].size(), cap, 0};
    FlowEdge b = {u, (int)adj[u].size(), 0, 0}; //reverse edge

    adj[u].push_back(a);
    adj[v].push_back(b);

    if(cap > max_capacity) max_capacity = cap;

}

bool FlowNetwork::bfs(int s, int t, double delta){

        if(level.size() != n) level.resize(n);
        std::fill(level.begin(), level.end(), -1);


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

    //max_capacity is already cached from add_edge calls

    //for (double delta = pow(2, floor(log2(max_capacity))); delta >= 1e-9; delta/=2){

        while(bfs(s, t, 0)) {
            if(ptr.size() != n) ptr.resize(n);
            std::fill(ptr.begin(), ptr.end(), 0);
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

    fo << "\n--- MATCHING_OR_CUT CALLED ---\n";
    log_graph_state(G, "Before flow computation");

    int n = G.nr_nodes;
    int s = n;
    int t = n + 1;

    //make flow network out of the graph
    FlowNetwork F = build_flow_network(G);

    //double threshold = G.L.size() * (1 - epsilon);
    double threshold = mu * (1 - 5 * epsilon);
    double flow = F.max_flow(s, t);

    current_flow = flow;

    log_matching_or_cut_result(flow, threshold, flow >= threshold);

    if(flow >= threshold) {
        return flow;
    }



    // Extract cut via BFS from source in residual graph
    fo << "\n  Extracting cut via BFS in residual graph...\n";
    vector<bool> reachable(F.n, false);

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

    // Build cut sets SL and SR
    fill(SL.begin(), SL.end(), false);
    fill(SR.begin(), SR.end(), false);

    for(int u : G.L){
        if(reachable[u]){
            SL[u] = true;
        }
    }
    for(int u : G.R){
        if(reachable[u]){
            SR[u] = true;
        }
    }

    log_cut_info(G, SL, SR);
    return -1;


}

bool matching_too_small(BipartiteGraph& G, double& mu, double epsilon){

    fo << "\n--- CHECKING IF MATCHING TOO SMALL ---\n";

    int L = (int)ceil(1.0 / epsilon );
    double mu_approx = HK_approx(G, L);

    fo << "  Using Hopcroft-Karp approximation with L = " << L << "\n";
    fo << "  Approximate matching size (mu_approx): " << mu_approx << "\n";
    fo << "  Original mu: " << mu << "\n";
    fo << "  Threshold (mu * (1 - 2*epsilon)): " << mu * (1 - 2*epsilon) << "\n";

    bool too_small = (mu_approx < mu*(1 - 2*epsilon));
    fo << "  Result: " << (too_small ? "TOO SMALL - algorithm terminates" : "OK - proceed with robust matching") << "\n";

    return too_small;

}

int robust_matching(BipartiteGraph& G, double& mu, double epsilon, int& doublings){

    log_separator("ROBUST MATCHING");
    log_graph_state(G, "Initial state");

    //if matching too small we return
    if(matching_too_small(G, mu, epsilon)){
        return -1;
    }

    //initialize the cuts as bool
    vector <bool> SL (G.nr_nodes, false);
    vector <bool> SR (G.nr_nodes, false);

    double flow = 0;
    double prev_flow = -1;
    double current_flow = 0;
    int iteration = 0;

    //while we do not find a matching
    while (true) {
        iteration++;
        fo << "\n>>> CONGESTION BALANCING ITERATION " << iteration << " <<<\n";

        int doublings_before = doublings;
        bool doubled_any = false;

        double flow = matching_or_cut(G, mu, epsilon, SL, SR, current_flow);

        if(flow >= 0 ){
            fo << "\n✓ SUCCESS: Found valid flow >= threshold!\n";
            fo << "  Final flow: " << flow << "\n";
            fo << "  Iterations needed: " << iteration << "\n";
            return 1;
        }

        prev_flow = current_flow;

        // Double capacities on edges crossing the cut (SL → R\SR)
        fo << "\n  Doubling capacities on edges from SL to R\\SR...\n";

        for (int u : G.L) {
            if(!SL[u]) continue;
            for(auto &e : G.edge_list[u]) {
                int v = e.to;

                if(G.is_right[v] && !SR[v]) {
                    e.capacity *= 2.0;
                    doublings++;
                    doubled_any = true;
                }
            }
        }

        int doublings_this_iter = doublings - doublings_before;
        log_doublings(doublings, doublings_this_iter);

        if(!doubled_any){
            fo << "\n✗ TERMINATION: No edges to double - cannot improve further\n";
            return 0;
        }

        log_graph_state(G, "After doubling capacities");
    }

    return 0;

}

