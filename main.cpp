#include <iostream>
#include <string>
#include <fstream>
#include "bipartite.h"
#include "helpers.h"
#include "congestion_balancing.h"


using namespace std;


ifstream fi("remove3.txt");

int main(){



    string test_name;

    cout << "please enter your test file: ";
    cin >> test_name;

    BipartiteGraph G(test_name);

    cout<<"Graph successfully loaded;\n";

    //let's test with deletions


    double epsilon, mu;

    cout<<"please enter epsilon: ";
    cin>>epsilon;

    int phase_counter = 0;

    mu =  hopcroft_karp(G);

    cout<<"This is before phase start just initial data:\n";
    cout<<"The maximum matching found is: " << mu;
    cout<<"\n\n";



    double value = 0;

    while(!fi.eof()){
        int u, v;
        fi >> u >> v;



        value += G.remove_edge(u, v);

        
        if(value >= epsilon * mu){
            //start phase
            cout<<"Start phase "<<++phase_counter;
            print_graph(G);

            robust_matching(G, mu, epsilon);


            value = 0;
        }

        //print_graph(G);
    }

    fi.close();

    return 0;
}