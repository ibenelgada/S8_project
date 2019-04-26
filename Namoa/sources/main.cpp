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

// 281027751; //place de la bourse
// 2518636235; //arts et metiers
// 3726441089; //arcachon
// 1902405360; //mériadeck
// 280464616; //peixotto
// 4304767162; //France alouette
// 266601918; // La cité du vin
// 288351743; //Saint médard
// 2466467260; //libourne
// 277341308; //mérignac aeroport

int main(int argc, char* argv[]){

  // string graph_file(argv[1]);
  // string nodes_file(argv[2]);
  // string in_file(argv[3]);
  // string out_file(argv[4]);



  string graph_file("data/graphWalk.cr");
  string nodes_file("data/nodes.co");
  string in_file("data/userInput.json");
  string out_file("data/output.json");

  ofstream o_file;

  Graph myGraph;
  map<long long,Position> nodes;

  long long start_node, end_node;


  init_graph_complete(myGraph, nodes, graph_file, nodes_file);
  get_start_end(in_file, start_node, end_node);

  string result = Namoa(myGraph, nodes, start_node, end_node);


  o_file.open(out_file);
  o_file <<  result << endl;
  o_file.close();

  return 0;
}
