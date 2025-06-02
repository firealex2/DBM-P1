//bipartite.cpp
#include <fstream>
#include <string>
#include <vector>
#include "bipartite.h"



using namespace std;


//initalization from file
BipartiteGraph::BipartiteGraph(string file_name){

    ifstream fi(file_name);


    // here we are going to read the number of nodes and then the edges
    nr_nodes = nr_edges = 0;

    int n;
    fi >> n;

    nr_nodes=n;

    for(int i=0;i<n;i++){
        
        vector<Edge> edge;
        edge_list.push_back(edge);


        Node new_node;
        new_node.id = i;
        new_node.partition = 0;

        nodes.push_back(new_node);


    }

    while(!fi.eof()){

        int u, v;
        fi>> u >> v;

        add_edge(u, v, 0);


        //repartition in left
        if(nodes[u].partition != 1 && nodes[v].partition != 1){
            nodes[u].partition = 1;
            L.push_back(u);
        }

        nr_edges++;

    }

    for(int i=0; i<n; i++){
        if (nodes[i].partition == 0)
        {
            R.push_back(i);
        }
    }

    int L_n = L.size();

    for(int i = 0; i < nr_nodes; i++){
            
        for(Edge& e : edge_list[i]){
            e.capacity = 1.0 / L_n;
        }    

    }


    fi.close();

};


//initialization with number of nodes
BipartiteGraph::BipartiteGraph(int n){

    for(int i=0;i<n;i++){
        vector<Edge> edge;
        edge_list.push_back(edge);
        

        Node new_node;
        new_node.id = i;
        new_node.partition = 0;

        nodes.push_back(new_node);
    }

};


// add edge to graph
void BipartiteGraph::add_edge(int u, int v , double capacity){

    //TODO: add error if edge already exists

    Edge edge_1 = {v, capacity, 0.0, 0};
    Edge edge_2 = {u, capacity, 0.0, 0};

    edge_list[u].push_back(edge_1);
    edge_list[v].push_back(edge_2);

    nodes[u].incident_edges[v] = edge_list[u].size() - 1;
    nodes[v].incident_edges[u] = edge_list[v].size() - 1;

}

double BipartiteGraph::remove_edge(int u, int v) {
    // Look up the stored index of (u→v)
    auto it_uv = nodes[u].incident_edges.find(v);
    if (it_uv == nodes[u].incident_edges.end() || it_uv->second < 0) {
        // either it never existed or was already removed
        return 0.0;
    }
    int idx_uv = it_uv->second;
    double cap = edge_list[u][idx_uv].capacity;

    // “Disable” u→v by setting capacity to 0 and marking removed
    edge_list[u][idx_uv].capacity = 0.0;
    it_uv->second = -1;

    // Now do the same for the reverse entry (v→u)
    auto it_vu = nodes[v].incident_edges.find(u);
    if (it_vu != nodes[v].incident_edges.end() && it_vu->second >= 0) {
        int idx_vu = it_vu->second;
        edge_list[v][idx_vu].capacity = 0.0;
        it_vu->second = -1;
    }

    --nr_edges;
    return cap;
}