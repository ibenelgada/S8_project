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

#include "functions.h"

#include "Position.h"
#include "Label.h"
#include "Cost.h"
#include "Arc.h"
#include "Node.h"
#include "Graph.h"

#include "Json.h"

using namespace std;
using namespace chrono;

bool vector_contains(const vector<long long>& v, long long val){
  for(unsigned int i=0; i < v.size(); ++i){
    if(v[i] == val)
      return true;
  }
  return false;
}


double getDistance(std::map<long long,Position> &nodes, long long nd1, long long nd2){

  Position &p1 = nodes[nd1];
  Position &p2 = nodes[nd2];

  double x1 = M_PI * p1.lat / 180;
  double y1 = M_PI * p1.lon / 180;
  double x2 = M_PI * p2.lat / 180;
  double y2 = M_PI * p2.lon / 180;

  double x = x2-x1;
  double y = y2-y1;

  double a = pow(sin(x/2) , 2) + cos(x1) * cos(x2)* pow(sin(y/2) , 2);
  double c = 2 * atan2 (sqrt(a) , sqrt(1-a));

  return 6378137 * c;

}

double getHeightDiff(std::map<long long,Position> &nodes, long long nd1, long long nd2){
  return nodes[nd2].alt - nodes[nd1].alt > 0 ? nodes[nd2].alt - nodes[nd1].alt : 0;
}

Cost getCost(std::map<long long,Position> &nodes, long long nd1, long long nd2){
  Cost g(0,0);
  g.distance = getDistance(nodes, nd1, nd2);
  g.height_diff = getHeightDiff(nodes, nd1, nd2);

  return g;
}

void filter(list <pair <Label*, map<long long,Cost> > >& open_all, map<long long, list<Label*>>& open, Label* lbp){

    list <pair <Label*, map<long long,Cost> > >::iterator it;
    Label* lbp_tmp;
    it = open_all.begin();
    while(it != open_all.end()){

        lbp_tmp = it->first;
        if(lbp->g < it->second[lbp->node]){
            it->second.erase(lbp->node);
            if(it->second.empty()){
              it = open_all.erase(it);
              open[lbp_tmp->node].remove(lbp_tmp);
              continue;
            }
        }

        ++it;
    }
}

bool dominated(map<long long, Cost> &eval, map<long long, list<Label*>>& label_list){

  for(map<long long, list<Label*>>::iterator it = label_list.begin(); it != label_list.end(); ++it){
      for(list<Label*>::iterator it_l = it->second.begin(); it_l != it->second.end(); ++it_l){
        if((*it_l)->g < eval[it->first]){
          eval.erase(it->first);
          break;
        }
      }
  }

  return eval.empty();
}

void rmDominated(std::list<std::pair<Label*,map<long long,Cost>>>& labels, Label* lbp){
    std::list<std::pair<Label*,map<long long,Cost>>>::iterator it;
    it = labels.begin();
    Label* lbp_tmp;

    while(it != labels.end()){
        lbp_tmp = it->first;
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

void rmDominated(std::list<Label*>& labels, Label* lbp){
    std::list<Label*>::iterator it;
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

bool isNondom(std::list<std::pair<Label*,map<long long,Cost>>>& open_all, Cost f){
  
    Cost f_tmp;
    for(std::list<std::pair<Label*,map<long long,Cost>>>::iterator it = open_all.begin(); it!=open_all.end(); ++it){
      for(map<long long, Cost>::iterator it_c = it->second.begin(); it_c != it->second.end(); ++it_c){
        f_tmp = it_c->second;
        if(f_tmp < f){
          return false;
        }
      }
    }
    return true;
}

bool isNondom(std::list<Label*>& open, Label* label){

  std::list<Label*>::const_iterator it;

  for(it=open.cbegin(); it!=open.cend(); ++it){
      if((*it)->g < label->g)
        return false;
  }

  return true;
}

std::list<std::pair<Label*,map<long long,Cost>>>::iterator getNondomNode(std::list<std::pair<Label*,map<long long,Cost>>>& open_all){

  for(std::list<std::pair<Label*,map<long long,Cost>>>::iterator it = open_all.begin(); it!=open_all.end(); ++it){
    for(map<long long, Cost>::iterator it_c = it->second.begin(); it_c != it->second.end(); ++it_c){

      if(isNondom(open_all,it_c->second)){
        return it;
      }

    }
  }

  return open_all.begin();
}

bool add_nondom(map<long long, list<Label*>>& best_labels, Label* lbp){
  list<Label*> &labels = best_labels[lbp->node];
  std::list<Label*>::iterator it;
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

void remove( std::vector<long long>& vect, long long id){

  for(std::vector<long long>::iterator it = vect.begin(); it != vect.end(); ++it){
    if(*it == id){
      vect.erase(it);
      return;
    }
  }

}

std::vector<long long> getNodes(Label* label){
  std::vector<long long> v;
  Label* label_ptr = label;
  while(label_ptr != nullptr){
    v.push_back(label_ptr->node);
    label_ptr = label_ptr->prev_label;
  }
  return v;
}

std::string to_json(std::list<Label*>& labels){

  if(labels.size() == 0){
    return "{}";
  }

  Object obj;
  Array itineraries;
  Object* itinerary;
  Object* criteria;
  Array* sections;
  Object* section;
  Array* nodes;
  std::vector<long long> nodes_vect;
  obj.add("nb_itineraries", new Integer(labels.size()));
  std::list<Label*>::iterator it;
  Label* label;

  for(it = labels.begin(); it != labels.end(); ++it){
    itinerary = new Object();
    label = *it;
    criteria = new Object();

    criteria->add("distance", new Integer(label->g.distance));
    criteria->add("time", new Integer(0));
    criteria->add("price", new Real(0));
    criteria->add("height", new Integer(label->g.height_diff));
    criteria->add("connections", new Integer(0));
    criteria->add("co2" , new Integer(0));
    criteria->add("effort", new Integer(0));

    itinerary->add("criteria", criteria);

    sections = new Array();

    for(int i=0; i<1; ++i){
      section = new Object();
      nodes = new Array();

      nodes_vect = getNodes(label);
      for(unsigned int i=0; i < nodes_vect.size(); ++i){
        nodes->arr.push_back(new Integer(nodes_vect[i]));
      }

      section->add("nodes", nodes);
      section->add("start", new String("17:30"));
      section->add("end", new String("17:30"));
      section->add("idTransport", new Integer(-1));
      section->add("public", new Boolean(false));
      section->add("type", new String("walking"));

      sections->arr.push_back(section);
    }

    itinerary->add("sections", sections);

    itineraries.arr.push_back(itinerary);
  }

  obj.add("itineraries", &itineraries);

  return obj.toString();
}


void init_graph_complete(Graph& myGraph, std::map<long long,Position> & nodes, std::string graph_file_str, std::string nodes_file_str){

  std::ifstream graph_file, nodes_file;
  graph_file.open(graph_file_str);
  nodes_file.open(nodes_file_str);

  std::string s;
  char tmp_char;
  long long nb_nodes_left = 1;
  long long id;
  char initial;

  long long nb_arcs_left = 1;

  long long nd1, nd2;
  double dist;
  double h_diff;
  double effort;
  double time;
  double co2;
  double price;

  while(nb_arcs_left){

    graph_file >> initial;


    if(initial == 'c'){
      getline(graph_file, s);
      continue;
    }

    if(initial == 'p'){
      graph_file >> s;
      graph_file >> nb_nodes_left >> nb_arcs_left;
      continue;
    }

    graph_file >> nd1 >> nd2 >> tmp_char >> time >> tmp_char >> dist >> tmp_char >> co2 >> tmp_char >>  effort >> tmp_char >> h_diff >> tmp_char >> price >> tmp_char;

    myGraph.nodes[nd1][nd2].cost = Cost(dist,0);

    --nb_arcs_left;
  }


  while(nb_nodes_left){

    nodes_file >> initial;

    if(initial == 'c'){
      getline(nodes_file, s);
      continue;
    }

    if(initial == 'p'){
      nodes_file >> s;
      nodes_file >> s;
      nodes_file >> s;
      nodes_file >> nb_nodes_left;
      continue;
    }

    if(initial == 'v'){
      nodes_file >> id;
      nodes[id] = Position();

      nodes_file >> nodes[id].lon >> nodes[id].lat >> nodes[id].alt;
      getline(nodes_file, s);
      --nb_nodes_left;
    }
  }

}


void get_start_end(const std::string& in_file, long long& start_node, long long& end_node){

    Object* obj = (Object*) jsonParseFile(in_file);

    Real* n1 = (Real*) obj->obj["start"];
    Real* n2 = (Real*) obj->obj["dest"];


   start_node = n1->val;
   end_node = n2->val;

}