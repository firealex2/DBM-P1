
#include <sstream>
#include <string>
#include <climits>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <fstream>
#include <random>
#include <functional>

#include "helpers.h"

using namespace std;


void write_random_bipartite(const string &filename, int n, double p) {

    //this function will generate random bipartite graphs for testing
    //i am using p as a probability to include an edge in the graph test


    mt19937_64 rng(random_device{}());

    uniform_real_distribution<double> U(0, 1);

    ofstream fout(filename);

    fout<< (2*n) << '\n';

    for(int u=0; u < n; u++){
        for(int v = n; v < 2*n; v++){
            if(U(rng) < p){
                fout<< u << " " << v << "\n";
            }
        }
    }

    fout.close();

}

void write_random_removals(const string &graph_fn, const string &removals_fn)
{
    mt19937_64 rng(random_device{}());
    ifstream fin(graph_fn);
    int total_nodes;
    fin >> total_nodes;

    vector<pair<int,int>> edges;
    int u,v;
    while (fin >> u >> v) {
        edges.emplace_back(u,v);
    }
    fin.close();

    shuffle(edges.begin(), edges.end(), rng);

    ofstream fout(removals_fn);
    for (auto &e : edges) {
        fout << e.first << " " << e.second << "\n";
    }
    fout.close();
}

int main() {



    for(auto n: {256, 512, 1024, 2048}) {
        for(auto p: {0.1, 0.2, 0.5, 0.8}) {
            ostringstream fn;
            fn<<"rand_n"<<n<<"_p"<<int(p*100)<<".txt";
            write_random_bipartite(fn.str(), n, p);

            ostringstream rfn;
            rfn << "rand_n" << n << "_p" << int(p*100) << "_removals.txt";
            string rem_fn = rfn.str();

            //write random deletions
            write_random_removals(fn.str(), rem_fn);

        }
    }

    return 0;


}