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

#include "Position.h"
#include "Label.h"
#include "Cost.h"

using namespace std;

double getDistance(map<int,Position> nodes, int nd1, int nd2){
  double x1 = nodes[nd1].lat;
  double y1 = nodes[nd1].lon;
  double x2 = nodes[nd2].lat;
  double y2 = nodes[nd2].lon;

  return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));

}

double getHeightDiff(map<int,Position> nodes, int nd1, int nd2){
  return nodes[nd2].alt - nodes[nd1].alt;
}


Cost getCost(map<int,Position> nodes, int nd1, int nd2){
  Cost g;
  g.distance = getDistance(nodes, nd1, nd2);
  g.height_diff = getHeightDiff(nodes, nd1, nd2);
  return g;
}

bool isNondom(list<Label*>& open, list<Label*>::iterator lb_it){
    Label* lbp = *lb_it;
    Label lb = *lbp;

    for(list<Label*>::iterator it= open.begin(); it != open.end(); ++it){
      lbp = *it;
      if(*lbp < lb)
        return false;
    }

    return true;
}

list<Label*>::iterator getNondomNode(list<Label*>& open){

  for(list<Label*>::iterator it = open.begin(); it!=open.end(); ++it){
    if( isNondom(open,it))
      return it;

  }
  return open.begin();
}

bool add_nondom(list<Label*>& labels, Label* lbp){
  list<Label*>::iterator it;
  Label lb,lb_tmp;
  Label* lbp_tmp;

  lb = *lbp;

  it = labels.begin();
  while(it != labels.end()){
    lbp_tmp = *it;
    lb_tmp = *lbp_tmp;

    if( lb_tmp < lb || lb_tmp == lb )
      return false;
    if( lb_tmp > lb)
      it = labels.erase(it);
    else
      ++it;
  }
  labels.push_back(lbp);
  return true;
}

bool produce_cycle(int m, Label* current_label){

  Label* label_ptr = current_label;
  while(label_ptr != nullptr){
    label_ptr = label_ptr->prev_label;
    if(label_ptr->node == m)
      return true;
  }

  return false;
}

int main(){

  map<int,vector<int>> graph;
  map<int,Position> nodes;

  int start_node = 1;
  int end_node = 4;

  int nb_nodes, nb_nodes_graph, id, nb_neighbours, neighbour_id;

  ifstream graph_file, nodes_file;

  graph_file.open("data/graph.txt");
  nodes_file.open("data/nodes.txt");

  nodes_file >> nb_nodes;

  for(int i=0; i < nb_nodes; ++i){
    nodes_file >> id;
    nodes[id] = Position();
    nodes_file >> nodes[id].lat;
    nodes_file >> nodes[id].lon;
    nodes_file >> nodes[id].alt;
  }

  graph_file >> nb_nodes_graph;

  for(int i=0; i<nb_nodes_graph; ++i){
    graph_file >> id;
    graph_file >> nb_neighbours;

    graph[id] = vector<int>();

    for(int j=0; j< nb_neighbours; ++j){
      graph_file >> neighbour_id;
      graph[id].push_back(neighbour_id);
    }
  }

  graph_file.close();
  nodes_file.close();


  list<Label*> closed;
  list<Label*> open;

  Label* label_tmp;
  int m;

  label_tmp = new Label();
  label_tmp->g = Cost(0.0,0.0);
  label_tmp->h = Cost(0.0,0.0);
  label_tmp->node = start_node;
  label_tmp->prev_label = nullptr;

  open.push_back(label_tmp);

  list<Label*>::iterator node_it;
  Label* current_label;
  list<Label*> best_labels;

  while(!open.empty()){
    //choosing a node from open
    node_it = getNondomNode(open);

    //moving node from open to closed
    current_label = *node_it;
    open.erase(node_it);
    closed.push_back(current_label);

    //processing path (node - Label)
    if(current_label->node == end_node){
      //if destination reached
      if(add_nondom(best_labels, current_label)){
        //add_nondom label to best_labels, and filter list open
        //filter(open, current_label);
        continue;
      }
    }

    /* Path EXPANSION */

    //neighbours
    vector<int>& neighbours = graph[current_label->node];

    for(int i = 0; i<neighbours.size(); ++i){
      m = neighbours[i];

      if(produce_cycle(m, current_label))
        continue;

      //calculate cost to m
      Cost cost_m;
      cost_m = current_label->g  + getCost(nodes, current_label->node, m);

    }


  }

  return 0;
}
