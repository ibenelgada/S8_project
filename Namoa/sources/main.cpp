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

int random_0_to_n(int n){
  return rand() % n;
}

void  get_rand_start_end(map<long long,Position> &nodes,long long & start_node, long long &end_node){

  double max_lat = -0.540042;
  double max_lon = 44.875729;
  double min_lat = -0.643030;
  double min_lon = 44.792914;

  map<long long, Position>::iterator it;

  it = nodes.begin();
  advance(it, random_0_to_n(nodes.size()));

  Position p = it->second;
  while( !(min_lat < p.lat && p.lat < max_lat && min_lon < p.lon && p.lon < max_lon)){
    it = nodes.begin();
    advance(it, random_0_to_n(nodes.size()));
    p = it->second;
  }

  start_node = it->first;

  it = nodes.begin();
  advance(it, random_0_to_n(nodes.size()));
  p = it->second;
  while( !(min_lat < p.lat && p.lat < max_lat && min_lon < p.lon && p.lon < max_lon)){
      ++it;
      p = it->second;
  }
  end_node = it->first;

}

int main(int argc, char* argv[]){

  srand(time(NULL));
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

  double etime, distance = 0;
  int treated(0);
  int found(0);
  int path_size(0);

  int i(0);

  init_graph_complete(myGraph, nodes, graph_file, nodes_file);
//   get_start_end(in_file, start_node, end_node);
// string s = Namoa(myGraph, nodes, start_node, end_node, etime, distance, treated, found, path_size);
// cout << i << ") " << etime << " " << treated << " " << found << " " << distance << " " << path_size << " " << start_node << " " << end_node << endl;
//
//   o_file.open(out_file);
//   o_file << s;
//   o_file.close();
// return 0;

while(true){
  ++i;
  get_rand_start_end(nodes, start_node, end_node);
  Namoa(myGraph, nodes, start_node, end_node, etime, distance, treated, found, path_size);
  cout << i << ") " << etime << " " << treated << " " << found << " " << distance << " " << path_size << " " << start_node << " " << end_node << endl;

}

  return 0;
}
