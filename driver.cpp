#include "Graph.h"





int main() {
    
    cout << "\n_____-(Start)-_____\n";
    Graph graph;
    graph.SpawnChildren(graph.root);
    cout << "\n------_(END)_------\n";
    graph.memo.printUniques();

    return 0;
}