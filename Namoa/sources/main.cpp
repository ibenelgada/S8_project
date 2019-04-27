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

#define ELAPSED_TIME      //uncomment to print the elapsed time of the algorithm

using namespace std;
using namespace chrono;

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

  double h_factor = 1.0;

  Graph myGraph;
  map<long long,Position> nodes;


  list <pair <Label*, map<long long,Cost> > > open_all;
  map<long long, list<Label*>> open;
  map<long long, list<Label*>> closed;

  map<long long, list<Label*>> best_labels;


  Label* label_tmp;

  list<pair <Label*, map<long long, Cost> > >::iterator node_it;
  Label* current_label = NULL;
  long long m;
  Cost cost_m;
  map<long long, Cost> eval_m;
  map<long long, Cost> heuristic_m;
  Label* new_label = NULL;
  map<long long, Cost> new_eval;

  long long start_node, end_node;
  vector<long long> dests;
//280464616
  init_graph_complete(myGraph, nodes, graph_file, nodes_file);

//  get_start_end(in_file, start_node, end_node);

  start_node = 281027751;

  dests.push_back(1902405360);
  dests.push_back(2518636235);
  dests.push_back(288351743);

  label_tmp = new Label(start_node);
  new_eval[1902405360] = getCost(nodes, start_node, 1902405360);

  label_tmp = new Label(start_node);
  new_eval[2518636235] = getCost(nodes, start_node, 2518636235);

  label_tmp = new Label(start_node);
  new_eval[288351743] = getCost(nodes, start_node, 288351743);


  open_all.push_back(pair<Label*, map<long long, Cost>>(label_tmp, new_eval));
  open[start_node].push_back(label_tmp);

  #ifdef ELAPSED_TIME
    auto start = high_resolution_clock::now();
  #endif

int counter(0);

  while(!open_all.empty()){
    ++counter;

    //choosing a node from open
    node_it = getNondomNode(open_all);

    //moving node from open to closed
    current_label = node_it->first;
    open_all.erase(node_it);
    open[current_label->node].remove(current_label);
    closed[current_label->node].push_back(current_label);

//    std::cout << (*node_it).first->node << std::endl;
//    std::cout << counter << " " << (*node_it).first->g.distance << " " << (*node_it).second.distance - (*node_it).first->g.distance  << std::endl;

    // string str;
    // cin >> str;

    //processing path (node - Label)
//    if(current_label->node == end_node){
      if(vector_contains(dests, current_label->node)){
      //if destination reached

        if(add_nondom(best_labels, current_label)){

        //add_nondom label to best_labels, and filter list open
        filter(open_all, open, current_label);
        continue;
      }
    }


    /* Path EXPANSION */

    //neighbours

      map<long long, Arc>& neighbours_arcs = myGraph.nodes[current_label->node];

      for(map<long long, Arc>::iterator it= neighbours_arcs.begin(); it != neighbours_arcs.end() ; ++it){

        m = it->first;

      if(produce_cycle(m, current_label)){
          continue;
        }

        //calculate cost to m

        cost_m = current_label->g  + neighbours_arcs[m].cost;
        new_label = new Label();
        new_label->node = m;
        new_label->g = cost_m;
        new_label->prev_label = current_label;

         if( (open.find(m) == open.end() || open.find(m)->second.empty() ) && (closed.find(m) == closed.end() || closed.find(m)->second.empty() )) {


           for(unsigned int i=0; i< dests.size(); ++i){
              heuristic_m[dests[i]] = getCost(nodes,m,dests[i]);
              heuristic_m[dests[i]].distance*=h_factor;
              eval_m[dests[i]] = cost_m + heuristic_m[dests[i]];   //F(m)
            }

          if(dominated(eval_m, best_labels)){
            continue;
          }

          open_all.push_back(pair<Label*,map<long long, Cost>>(new_label,eval_m));
          open[m].push_back(new_label);

        }else{
        //if g_m is non-dominated by any cost vectors in G_op(m) ∪ G_cl(m)
        //(a path to m with new long longeresting cost has been found)

        if(!isNondom(open[m], new_label) || !isNondom(closed[m], new_label)){
          continue;
        }

        for(unsigned int i=0; i< dests.size(); ++i){
           heuristic_m[dests[i]] = getCost(nodes,m,dests[i]);
           heuristic_m[dests[i]].distance*=h_factor;
           eval_m[dests[i]] = cost_m + heuristic_m[dests[i]];   //F(m)
         }

        rmDominated(open_all, new_label);
        rmDominated(open[m], new_label);
        rmDominated(closed[m], new_label);

        if(dominated(eval_m, best_labels)){
          continue;
        }

          open_all.push_back(pair<Label*,map<long long, Cost>>(new_label,eval_m));
          open[m].push_back(new_label);


      } //if

    } //for

  } //while


  #ifdef ELAPSED_TIME
    auto end = high_resolution_clock::now();
    std::cout << "time elapsed " << duration_cast<microseconds>(end - start).count() / 1000000.0 << "s" << std::endl;
  #endif

  o_file.open(out_file);
  o_file << to_json(best_labels[end_node]) << endl;
  o_file.close();

  cout << best_labels.size() << endl;

  // for(map<long long, list<Label*>>::iterator it = best_labels.begin(); it != best_labels.end(); ++it){
  //   cout << it->first << " " << (*it->second.begin())->g.distance << endl;
  // }

  return 0;
}
