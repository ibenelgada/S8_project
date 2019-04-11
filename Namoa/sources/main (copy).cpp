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
#include "Arc.h"
#include "Node.h"
#include "Graph.h"

//#define DEBUG
#define SIZE

#define TAB "-----------------------"

using namespace std;

Position mean(const map<long long,Position>& nodes){

  Position m;

  map<long long,Position>::const_iterator it;
  for(it = nodes.begin(); it != nodes.end(); ++it){
    m.lon = m.lon + it->second.lon;
    m.lat = m.lat + it->second.lat;
  }

  m.lon = m.lon / nodes.size();
  m.lat = m.lat / nodes.size();
  return m;

}

void dotGraph(const map<long long,vector<long long>>& graph, const map<long long,Position>& nodes, string dot_file_path){
  ofstream file;
  file.open(dot_file_path);
  file << "strict graph namoaGraph {\n";
  file << "\tgraph [pad=\"0.212,0.055\" bgcolor=lightgray]\n";
  file << "\tnode [width=30 shape=point fontsize=30 penwidth=3.0]\n";

  Position pos;
  string posx;
  string posy;

  vector<std::string> colors = {"blue", "forestgreen", "red", "darkgoldenrod1", "darkviolet", "darkorange1", "salmon"};

  string color;
  string rs;
  string c;

  long long id;

  c = colors[0];

  map <long long, bool> in_graph;
  map<long long,vector<long long>>::const_iterator itg;

  for(itg=graph.cbegin(); itg != graph.cend(); ++itg){
    id = itg->first;
    in_graph[id] = true;
    for(unsigned long long i=0; i<itg->second.size(); ++i){
        in_graph[itg->second[i]] = true;
    }
  }

  map<long long,Position>::const_iterator it;


  Position m = mean(nodes);

  for(it=nodes.cbegin(); it != nodes.cend(); ++it){

    pos = it->second;

    if(in_graph.find(it->first) == in_graph.end())
      continue;

    posx = to_string( (int) (( (pos.lon - m.lon) * 1000000 )) );
    posy = to_string( (int) (( (pos.lat - m.lat) * 1000000 )) );

    // if(abs((int) (( (pos.lon - m.lon) * 1000000 )/110)) > 30 || abs((int) (( (pos.lat - m.lat) * 1000000 )/110)) > 30){
    //   in_graph[it->first] = false;
    //   continue;
    // }

    file << "\t" << it->first << " [pos=\"" << posx << "," << posy << "!\" " << "color=" << c << "]\n";
  }

  for(itg=graph.cbegin(); itg != graph.cend(); ++itg){
    id = itg->first;

    for(unsigned long long i=0; i<itg->second.size(); ++i){
      if( (nodes.find(id) != nodes.end()) && (nodes.find(itg->second[i]) != nodes.end())){
        file << "\t" << id << " -- " << itg->second[i] << "[penwidth=300];\n";
      }
    }
  }

  file << "}";
  file.close();
}

void initGraph(map<long long,vector<long long>>& graph, map<long long,Position>& nodes, string graph_file_str, string nodes_file_str, string dot_file){


  long long nb_nodes, nb_nodes_graph, id, nb_neighbours, neighbour_id;

  ifstream graph_file, nodes_file;

  graph_file.open(graph_file_str);
  nodes_file.open(nodes_file_str);

  nodes_file >> nb_nodes;
  for(long long i=0; i < nb_nodes; ++i){
    nodes_file >> id;
    nodes[id] = Position();
    nodes_file >> nodes[id].lat;
    nodes_file >> nodes[id].lon;
    nodes_file >> nodes[id].alt;
  }

  graph_file >> nb_nodes_graph;

  for(long long i=0; i<nb_nodes_graph; ++i){
    graph_file >> id;
    graph_file >> nb_neighbours;

    graph[id] = vector<long long>();

    for(long long j=0; j< nb_neighbours; ++j){
      graph_file >> neighbour_id;
      graph[id].push_back(neighbour_id);
    }
  }

  graph_file.close();
  nodes_file.close();

  dotGraph(graph, nodes, dot_file);
}

double getDistance(map<long long,Position> &nodes, long long nd1, long long nd2){
  double x1 = nodes[nd1].lat;
  double y1 = nodes[nd1].lon;
  double x2 = nodes[nd2].lat;
  double y2 = nodes[nd2].lon;

  return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));

}

double getHeightDiff(map<long long,Position> &nodes, long long nd1, long long nd2){
  return nodes[nd2].alt - nodes[nd1].alt > 0 ? nodes[nd2].alt - nodes[nd1].alt : 0;
}

Cost getCost(map<long long,Position> &nodes, long long nd1, long long nd2){
  Cost g;
//  g.distance = getDistance(nodes, nd1, nd2);
//  g.height_diff = getHeightDiff(nodes, nd1, nd2);
  g.distance = 1;

  return g;
}

bool inList(long long node, list<Label*> &l){
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
  long long m = label->node;
  long long n;

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

bool produce_cycle(long long m, Label* current_label){

  Label* label_ptr = current_label;
  while(label_ptr != nullptr){
    if(label_ptr->node == m)
      return true;
    label_ptr = label_ptr->prev_label;
  }

  return false;
}

void print_list(const list<Label*> &labels){

  if(labels.empty()){
    cout << "      empty" << endl;
    return;
  }

  int i= 1;
  for(list<Label*>::const_iterator it = labels.cbegin(); it != labels.cend(); ++it){
    cout << "      " << i << ") " <<  **it << endl;
    ++i;
  }

}

void remove( vector<long long>& vect, long long id){

  for(vector<long long>::iterator it = vect.begin(); it != vect.end(); ++it){
    if(*it == id){
      vect.erase(it);
      return;
    }
  }

}

void truncate(map<long long, vector<long long>> &graph, map<long long, Position>&nodes, long long start_node, long long end_node, double eps){
  map<long long, vector<long long>>::iterator it;

  long long i,j,k;
  double ij, ik, jk;

  it = graph.begin();
  while( it != graph.end()){

    if(it->second.size() != 2 || it->first == start_node || it->first == end_node){
      ++it;
      continue;
    }

    // j - i - k

    i = it->first;
    j = it->second[0];
    k = it->second[1];

    ij = getDistance(nodes, i, j);
    ik = getDistance(nodes, i, k);
    jk = getDistance(nodes, j, k);

    if( abs( (ij+ik) - jk ) < eps){
      remove(graph[j], i);
      remove(graph[k], i);
      graph[j].push_back(k);
      graph[k].push_back(j);

      it = graph.erase(it);
      continue;
    }

    ++it;
  }
}

int main(){

  Graph myGraph;

  map<long long,vector<long long>> graph;
  map<long long,Position> nodes;

  long long end_node = 25186361913;
  long long start_node = 2292387593;

  //initGraph(graph, nodes,"data/graph.txt", "data/nodes.txt", "data/graph.dot");
  initGraph(graph, nodes,"../Data/data/talence_graph.txt", "../Data/data/talence_nodes.txt", "data/graph.dot");


  //truncate(graph, nodes, start_node, end_node, 1.3e-6);

  list<Label*> closed;
  list<Label*> open;

  Label* label_tmp;
  long long m;

  label_tmp = new Label(start_node);

  open.push_back(label_tmp);

  list<Label*>::iterator node_it;
  Label* current_label;
  list<Label*> best_labels;


  Position tmp;
  while(!open.empty()){

#ifdef SIZE
    cout << closed.size() << " " << open.size() << endl;
#endif
    //choosing a node from open
    node_it = getNondomNode(open);

#ifdef DEBUG
    cout << TAB << TAB << TAB << TAB << TAB << endl;
    cout << " - open is:" << endl;
    print_list(open);
    cout << " - closed is:" << endl;
    print_list(closed);
    cout << " - treating "<< **node_it << endl;
#endif
    //moving node from open to closed
    current_label = *node_it;
    open.erase(node_it);
    closed.push_back(current_label);

    //processing path (node - Label)
    if(current_label->node == end_node){
      //if destination reached

#ifdef DEBUG
      cout << " - destination reached." << endl;
      cout << "best labels is:" << endl;
      print_list(best_labels);
#endif

      if(add_nondom(best_labels, current_label)){

        //add_nondom label to best_labels, and filter list open
        filter(open, current_label);

#ifdef DEBUG
        cout << "label added, now best labels is:" << endl;
        print_list(best_labels);
        cout << "open after filtring:" << endl;
        print_list(open);
#endif

        continue;
      }
    }

    /* Path EXPANSION */

    //neighbours
    vector<long long>& neighbours = graph[current_label->node];
#ifdef DEBUG
    cout << " - path expansion" << endl;
#endif
    for(unsigned long long i = 0; i<neighbours.size(); ++i){
      m = neighbours[i];

#ifdef DEBUG
      cout << "      -node " << m << endl;
#endif
      if(produce_cycle(m, current_label)){
#ifdef DEBUG
        cout << "        produces cycle, passing" << endl;
#endif
        continue;
      }

      //calculate cost to m
      Cost cost_m;
      cost_m = current_label->g  + getCost(nodes, current_label->node, m);
      Cost heuristic_m(0,0);
//      heuristic_m = getCost(nodes,m,end_node);

      Label* new_label = new Label();
      new_label->node = m;
      new_label->g = cost_m;
      new_label->h = heuristic_m;
      new_label->prev_label = current_label;

      Cost eval_m = cost_m + heuristic_m;   //F(m)
#ifdef DEBUG
      cout << "        created " << *new_label << endl;
#endif
      //if m is a new node
      if(!inList(m, open) && !inList(m,closed)){

        if(dominated(eval_m, best_labels)){
#ifdef DEBUG
          cout << "        its dominated by destination" << endl;
#endif
          continue;
        }


        open.push_back(new_label);
#ifdef DEBUG
        cout << "        its a new label, added" << endl;
#endif

      }else{

        //if g_m is non-dominated by any cost vectors in G_op(m) ∪ G_cl(m)
        //(a path to m with new long longeresting cost has been found)
        if(!isNondom(open, new_label) && !isNondom(closed, new_label)){
#ifdef DEBUG
          cout << "        its dominated by better path" << endl;
#endif
          continue;
        }

        rmDominated(open, new_label);
        rmDominated(closed, new_label);

        if(dominated(eval_m, best_labels)){
#ifdef DEBUG
          cout << "        its dominated by destination" << endl;
#endif
          continue;
        }

        open.push_back(new_label);
#ifdef DEBUG
        cout << "        added to open." << endl;
#endif
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

  cout << best_labels.size() << endl;

  return 0;
}
