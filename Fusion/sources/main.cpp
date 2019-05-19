#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <map>
#include <climits>
#include <fstream>
#include <cmath>
#include <ctime>
#include <sstream>
#include <chrono>


#include "Position.h"
#include "Label.h"
#include "Cost.h"
#include "Arc.h"
#include "Node.h"
#include "Graph.h"
#include "functions.h"
#include "Json.h"


using namespace std;

int main(int argc, char* argv[]){

//  cout << time_str_to_n("15:10") << endl;

  // string graph_file(argv[1]);
  // string nodes_file(argv[2]);
  // string in_file(argv[3]);
  // string out_file(argv[4]);
  // long long start_node(stoll(argv[5])), end_node(stoll(argv[6]));

  long long start_node, end_node;

  double start_time;

  string graph_file("data/graphWalk_c.cr");
  string nodes_file("data/nodes_c.co");
  string in_file("data/userInput.json");
  string out_file("data/output.json");

  ofstream o_file;


  Graph myGraph;
  map<long long,Position> nodes;

  init_graph_complete(myGraph, nodes, graph_file, nodes_file);

  get_start_end(in_file, start_node, end_node, start_time);

  Network myNetwork("data/r.txt", "data/pathways.txt");

  string result = fusion(myGraph, nodes, myNetwork, start_node, end_node, start_time);

  cout << result << endl;

  return 0;
}
