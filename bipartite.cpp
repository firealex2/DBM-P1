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

    int n;
    fi >> n;

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

    }

    for(int i=0; i<n; i++){
        if (nodes[i].partition == 0)
        {
            R.push_back(i);
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

void BipartiteGraph::remove_edge(int u, int v){

    //TODO: add error if edge does not exist

    int index1 = nodes[u].incident_edges[v];
    int index2 = nodes[v].incident_edges[u];

    edge_list[u].erase(edge_list[u].begin() + index1);
    edge_list[v].erase(edge_list[v].begin() + index2);

    nodes[u].incident_edges[v] = -1;
    nodes[v].incident_edges[u] = -1;

}