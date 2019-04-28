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

#define ELAPSED_TIME      //uncomment to print the elapsed time of the algorithm


using namespace std;

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

  return 6371000 * c;

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

void filter(std::list<std::pair<Label*,Cost>>& open_all, std::map<long long, std::list<Label*>>& open, Label* lbp){

    std::list<std::pair<Label*,Cost>>::iterator it;
    Label* lbp_tmp;
    it = open_all.begin();
    while(it != open_all.end()){
        lbp_tmp = it->first;
        if(lbp->g < it->second){
            it = open_all.erase(it);
            open[lbp_tmp->node].remove(lbp_tmp);
            continue;
        }

        ++it;
    }
}

bool dominated(Cost eval, const std::list<Label*>& label_list){

  std::list<Label*>::const_iterator it;
  for(it = label_list.cbegin(); it!=label_list.cend(); ++it){
    if((*it)->g < eval)
      return true;
  }

  return false;
}

void rmDominated(std::list<std::pair<Label*,Cost>>& labels, Label* lbp){
    std::list<std::pair<Label*,Cost>>::iterator it;
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

bool isNondom(std::list<std::pair<Label*,Cost>>& open, std::list<std::pair<Label*,Cost>>::iterator lb_it){

    Cost f = lb_it->second;

    Cost f_tmp;

    for(std::list<std::pair<Label*,Cost>>::iterator it= open.begin(); it != open.end(); ++it){
      f_tmp = it->second;
      if(f_tmp < f){
        return false;
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

std::list<std::pair<Label*,Cost>>::iterator getNondomNode(std::list<std::pair<Label*,Cost>>& open_all){

  for(std::list<std::pair<Label*,Cost>>::iterator it = open_all.begin(); it!=open_all.end(); ++it){
    if(isNondom(open_all,it))
      return it;
  }
  return open_all.begin();
}

bool add_nondom(std::list<Label*>& labels, Label* lbp){
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

std::string Namoa(Graph& myGraph, std::map<long long, Position>& nodes, long long start_node, long long end_node){

    double h_factor = 1.0;


    Label* label_tmp;
    list<pair<Label*,Cost>> open_all;
    map<long long, list<Label*>> open;
    map<long long, list<Label*>> closed;
    list<Label*> best_labels;

    list<pair<Label*,Cost>>::iterator node_it;
    Label* current_label;
    long long m;
    Cost eval_m;
    Cost cost_m(0,0);
    Cost heuristic_m(0,0);
    Label* new_label = NULL;

    label_tmp = new Label(start_node);

    open_all.push_back(pair<Label*,Cost>(label_tmp,Cost(0,0)));
    open[start_node].push_back(label_tmp);

  #ifdef ELAPSED_TIME
    chrono::microseconds ms = chrono::duration_cast< chrono::microseconds >(chrono::system_clock::now().time_since_epoch());
    chrono::microseconds ms1;
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
  //    cout << counter << endl;
  //    std::cout << (*node_it).first->node << std::endl;
  //    std::cout << counter << " " << (*node_it).first->g.distance << " " << (*node_it).second.distance - (*node_it).first->g.distance  << std::endl;

      // string str;
      // cin >> str;

      //processing path (node - Label)
      if(current_label->node == end_node){
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

              heuristic_m = getCost(nodes,m,end_node);
              heuristic_m.distance*=h_factor;
              eval_m = cost_m + heuristic_m;   //F(m)

            if(dominated(eval_m, best_labels)){
              continue;
            }

            open_all.push_back(pair<Label*,Cost>(new_label,eval_m));
            open[m].push_back(new_label);

          }else{
          //if g_m is non-dominated by any cost vectors in G_op(m) ∪ G_cl(m)
          //(a path to m with new long longeresting cost has been found)

          if(!isNondom(open[m], new_label) || !isNondom(closed[m], new_label)){
            continue;
          }

          heuristic_m = getCost(nodes,m,end_node);
          heuristic_m.distance*=h_factor;
          eval_m = cost_m + heuristic_m;   //F(m)

          rmDominated(open_all, new_label);
          rmDominated(open[m], new_label);
          rmDominated(closed[m], new_label);

          if(dominated(eval_m, best_labels)){
            continue;
          }

            open_all.push_back(pair<Label*,Cost>(new_label,eval_m));
            open[m].push_back(new_label);


        } //if

      } //for

    } //while


  #ifdef ELAPSED_TIME
    ms1 = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch() );
    cout << "time elapsed " << (ms1.count() - ms.count()) / 1000000.0 << "s" << endl;
  #endif

    return to_json(best_labels);
}
