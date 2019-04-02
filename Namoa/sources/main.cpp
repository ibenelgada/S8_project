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

    if(current_label->node == end_node){
      add_nondom(best_labels, current_label);
      
    }



    //process node
  }

  return 0;
}
