// graph implementation bipartite.h

#pragma once
#include <vector>
#include <string>
#include <map>


struct Edge {
    int to;
    double capacity;
    double flow;
    int rev_index;
};


struct Node {

    int id;
    
    //1 if left and 0 if right
    int partition;
    //array that for each i returns the id in the global edges
    std::map<int, int> incident_edges;

    bool operator<(const Node& other) const {
        return id < other.id;
    }
    

};



class BipartiteGraph {

    // adjaceny list
    // edge structure
    // add/remove edge
    // build flow network
public:
    int nr_nodes;
    int nr_edges;

    //sets for left and right in bipartite graph
    std::vector<int> L, R;

    //adjacency edge list
    std::vector< std::vector<Edge> > edge_list;

    //list of nodes, 1 means left, 0 right
    std::vector<Node> nodes;

    // constructors
    BipartiteGraph(int n);
    BipartiteGraph(std::string file_name);

    void add_edge(int u, int v, double cap);
    double remove_edge(int u, int v);


    
};

