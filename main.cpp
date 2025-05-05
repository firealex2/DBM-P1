#include <iostream>
#include <string>
#include <fstream>
#include "bipartite.h"
#include "helpers.h"


using namespace std;


ifstream fi("remove3.txt");

int main(){

    string test_name;

    cout << "please enter your test file: ";
    cin >> test_name;

    BipartiteGraph G(test_name);

    cout<<"Graph successfully loaded;\n";

    //let's test with deletions

    cout<<"The maximum matching found is: " << hopcroft_karp(G);



    /*while(!fi.eof()){
        int u, v;
        fi >> u >> v;

        G.remove_edge(u, v);

        print_graph(G);
    }

    fi.close();*/

    return 0;
}