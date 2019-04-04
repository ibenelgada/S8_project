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

void dotGraph(const map<int,vector<int>>& graph, const map<int,Position>& nodes, string dot_file_path){
  ofstream file;
  file.open(dot_file_path);
  file << "strict graph namoaGraph {\n";
  file << "\tgraph [pad=\"0.212,0.055\" bgcolor=lightgray]\n";
  file << "\tnode [width=0.2 shape=circle fontsize=30 penwidth=3.0]\n";

  Position pos;
  string posx;
  string posy;

  vector<std::string> colors = {"blue", "forestgreen", "red", "darkgoldenrod1", "darkviolet", "darkorange1", "salmon"};

  string color;
  string rs;
  string c;

  int id;

  c = colors[0];

  map<int,Position>::const_iterator it;

  for(it=nodes.cbegin(); it != nodes.cend(); ++it){

    pos = it->second;
    posx = to_string(pos.lat);
    posy = to_string(pos.lon);

    file << "\t" << it->first << " [pos=\"" << posx << "," << posy << "!\" " << "color=" << c << "]\n";
  }

  map<int,vector<int>>::const_iterator itg;


  for(itg=graph.cbegin(); itg != graph.cend(); ++itg){
    id = itg->first;

    for(unsigned int i=0; i<itg->second.size(); ++i){
      file << "\t" << to_string(id) << " -- " << to_string(itg->second[i]) << ";\n";
    }

  }
  file << "}";
  file.close();
}

void initGraph(map<int,vector<int>>& graph, map<int,Position>& nodes, string graph_file_str, string nodes_file_str, string dot_file_path){


  int nb_nodes, nb_nodes_graph, id, nb_neighbours, neighbour_id;

  ifstream graph_file, nodes_file;

  graph_file.open(graph_file_str);
  nodes_file.open(nodes_file_str);

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


  dotGraph(graph, nodes, dot_file_path);
}


double getDistance(map<int,Position> nodes, int nd1, int nd2){
  double x1 = nodes[nd1].lat;
  double y1 = nodes[nd1].lon;
  double x2 = nodes[nd2].lat;
  double y2 = nodes[nd2].lon;

  return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));

}

double getHeightDiff(map<int,Position> nodes, int nd1, int nd2){
  return nodes[nd2].alt - nodes[nd1].alt > 0 ? nodes[nd2].alt - nodes[nd1].alt : 0;
}


Cost getCost(map<int,Position> nodes, int nd1, int nd2){
  Cost g;
  g.distance = getDistance(nodes, nd1, nd2);
  g.height_diff = getHeightDiff(nodes, nd1, nd2);
  return g;
}


bool inList(int node, list<Label*> l){
  list<Label*>::const_iterator it;

  for(it=l.cbegin(); it != l.cend(); ++it ){
    if((*it)->node == node)
      return true;
  }

  return false;
}

void filter(list<Label*>& labels, Label* lbp){
    list<Label*>::iterator it;
    Label* lbp_tmp;
    it = labels.begin();
    while(it != labels.end()){
        lbp_tmp = *it;
        if(lbp->g < (lbp_tmp->g + lbp_tmp->h)){
            it = labels.erase(it);
            continue;
        }

        ++it;
    }
}

bool dominated(Cost eval, const list<Label*>& label_list){

  list<Label*>::const_iterator it;
  for(it = label_list.cbegin(); it!=label_list.cend(); ++it){
    if((*it)->g + (*it)->h < eval)
      return true;
  }

  return false;
}

void rmDominated(list<Label*>& labels, Label* lbp){
    list<Label*>::iterator it;
    it = labels.begin();
    Label* lbp_tmp;

    while(it != labels.end()){
        lbp_tmp = *it;
        if(lbp_tmp->node != lbp->node){
            ++it;
            continue;
        }

        if(lbp_tmp->g > lbp->g){
            it = labels.erase(it);
            continue;
        }

        ++it;
    }
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

bool isNondom(list<Label*>& open, Label* label){

  list<Label*>::const_iterator it;
  int m = label->node;
  int n;

  for(it=open.cbegin(); it!=open.cend(); ++it){
      n = (*it)->node;
      if(n != m)
        continue;
      if((*it)->g < label->g)
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
    if(label_ptr->node == m)
      return true;
    label_ptr = label_ptr->prev_label;
  }

  return false;
}

int main(){

  map<int,vector<int>> graph;
  map<int,Position> nodes;

  int start_node = 1;
  int end_node = 4;

  initGraph(graph, nodes,"data/graph.txt", "data/nodes.txt", "data/graph.dot");

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
        filter(open, current_label);
        continue;
      }
    }

    /* Path EXPANSION */

    //neighbours
    vector<int>& neighbours = graph[current_label->node];

    for(unsigned int i = 0; i<neighbours.size(); ++i){
      m = neighbours[i];

      if(produce_cycle(m, current_label))
        continue;

      //calculate cost to m
      Cost cost_m;
      cost_m = current_label->g  + getCost(nodes, current_label->node, m);
      Cost heuristic_m(0,0);

      Label* new_label = new Label();
      new_label->node = m;
      new_label->g = cost_m;
      new_label->h = heuristic_m;
      new_label->prev_label = current_label;

      Cost eval_m = cost_m + heuristic_m;   //F(m)

      //if m is a new node
      if(!inList(m, open) && !inList(m,closed)){

        if(dominated(eval_m, best_labels))
          continue;


        open.push_back(new_label);

      }else{

        //if g_m is non-dominated by any cost vectors in G_op(m) ∪ G_cl(m)
        //(a path to m with new interesting cost has been found)
        if(!isNondom(open, new_label) && !isNondom(closed, new_label))
          continue;

        rmDominated(open, new_label);
        rmDominated(closed, new_label);

        if(dominated(eval_m, best_labels))
          continue;

        open.push_back(new_label);

      } //if

    } //for

  } //while

  list<Label*>::iterator it = best_labels.begin();
  //++it;
  Label* label_ptr = *it;

  while(label_ptr != nullptr){
    cout << label_ptr->node << " " ;
    label_ptr = label_ptr -> prev_label;
  }
  cout << endl;

  cout << (*it)->g.height_diff << endl;

  cout << best_labels.size() << endl;

  return 0;
}
