#include "bipartite.h"
#include <iostream>

using namespace std;


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