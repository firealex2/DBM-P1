#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "bipartite.h"
#include "helpers.h"
#include "congestion_balancing.h"
#include <chrono>


using namespace std;

ofstream fo;

ofstream results_csv;




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


    //do work for the opening csv

    ostringstream csv_name; 

    csv_name << "results_" << test_name << "_" << removal_name << "_e"<<epsilon << ".csv";

    results_csv.open(csv_name.str());

    results_csv << "n,p,epsilon,phases,doublings,time_ms\n";

    //get the n and p from the name of the file

    int nr_nodes, density_probability;

    sscanf(test_name.c_str(), "rand_n%d_p%d.txt", &nr_nodes, &density_probability);

    double density = density_probability / 100.0;

    //open the output file

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

    //print_graph(G);

    auto t0 = chrono::steady_clock::now();

    fo<<" << Start phase "<< phase_counter << " initial run \n ------------- \n";



    int doublings = 0;

    robust_matching(G, mu, epsilon, doublings);

    int u, v;

    //print_graph(G);

    

    while(fi>>u>>v){

        value += G.remove_edge(u, v);

        
        if(value >= epsilon * mu){
            //start phase
            fo<<" << Start phase "<<++phase_counter << ">> \n\n";

            fo<<"We enter robust matching: ...\n";


            int err = robust_matching(G, mu, epsilon, doublings);
            

            //if matching too small was too small
            if(err < 0){
                

                auto t1 = chrono::steady_clock::now();

                fo<<"Matching too small\n";

                double time_ms = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();

                results_csv << nr_nodes << "," << density << "," << epsilon << "," << phase_counter << "," << doublings << "," << time_ms <<  "\n";

                fi.close();
                fo.close();
                
                results_csv.close();
                return 1;
            }


            value = 0;
        }


    }

    auto t1 = chrono::steady_clock::now();
    double time_ms = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();


    results_csv << nr_nodes << "," << density << "," << epsilon << "," << phase_counter << "," << doublings << "," << time_ms <<  "\n";


    results_csv.close();
    fi.close();
    fo.close();

    return 0;
}