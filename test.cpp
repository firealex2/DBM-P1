#include <iostream>


int add(int& a){
    a++;
    return 1;
}

int main(){
    int v = 0;

    int it_did = add(v);
    it_did = add(v);
    std::cout<<v;
}