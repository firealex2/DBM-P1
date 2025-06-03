#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "bipartite.h"
#include "helpers.h"
#include "congestion_balancing.h"


using namespace std;

ofstream fo;


int main(int argc, char *argv[]){




    string test_name, removal_name;
    double epsilon, mu;

    argument_parser(argc, argv, test_name, removal_name, epsilon);


    BipartiteGraph G(test_name);
    ifstream fi(removal_name);
    if(!fi) {
        cerr<< " Failed to open removal file "<< removal_name << '\n';
        return 1;
    }

    fo<<"Graph successfully loaded;\n";


    ostringstream oss;
    oss << "i(" << test_name <<")_r("<<removal_name<<")_e(" << epsilon << ").out";

    string out_file = oss.str();

    fo.open(out_file);
    if(!fo) {
        cerr << "Error: Cannot open output file " << out_file << '\n';
        return 1;
    }

    int phase_counter = 0;

    mu =  hopcroft_karp(G);

    fo<<"This is before phase start just initial data:\n";
    fo<<"The maximum matching found is: " << mu;
    fo<<"\n\n";


    double value = 0;

    print_graph(G);


    fo<<"Start phase "<< phase_counter << " initial run \n ------------- \n";


    robust_matching(G, mu, epsilon);

    int u, v;

    print_graph(G);

    while(fi>>u>>v){

        value += G.remove_edge(u, v);

        
        if(value >= epsilon * mu){
            //start phase
            fo<<"Start phase "<<++phase_counter;

            fo<<"We enter robust matching: ...\n";


            robust_matching(G, mu, epsilon);


            value = 0;
        }


    }



    fi.close();
    fo.close();

    return 0;
}