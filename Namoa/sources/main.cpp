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


#include "Position.h"
#include "Label.h"
#include "Cost.h"
#include "Arc.h"
#include "Node.h"
#include "Graph.h"

#include "Json.h"

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

double getDiff(map<long long,Position> &nodes, long long nd1, double lon2, double lat2){
  double lon1 = nodes[nd1].lon;
  double lat1 = nodes[nd1].lat;

  return pow(lon1-lon2,2) + pow(lat1-lat2,2);
}

long long getNearest(map<long long,Position> &nodes, double lat, double lon){
  double d, tmp = 0;
  long long id;
  d = getDiff(nodes, nodes.begin()->first, lon, lat);
  id = nodes.begin()->first;
  map<long long, Position>::iterator it;
  for(it = nodes.begin(); it != nodes.end(); ++it){

    tmp = getDiff(nodes, it->first, lon, lat);
    if(tmp < d){
      d = tmp;
      id = it->first;
    }
  }
  return id;
}

double getDistance(map<long long,Position> &nodes, long long nd1, long long nd2){
  double x1 = M_PI * nodes[nd1].lat / 180;
  double y1 = M_PI * nodes[nd1].lon / 180;
  double x2 = M_PI * nodes[nd2].lat / 180;
  double y2 = M_PI * nodes[nd2].lon / 180;

  double x = x2-x1;
  double y = y2-y1;

  double a = pow(sin(x/2) , 2) + cos(x1) * cos(x2)* pow(sin(y/2) , 2);
  double c = 2 * atan2 (sqrt(a) , sqrt(1-a));

  return 6371000 * c;

}

double getHeightDiff(map<long long,Position> &nodes, long long nd1, long long nd2){
  return nodes[nd2].alt - nodes[nd1].alt > 0 ? nodes[nd2].alt - nodes[nd1].alt : 0;
}

Cost getCost(map<long long,Position> &nodes, long long nd1, long long nd2){
  Cost g;
  g.distance = getDistance(nodes, nd1, nd2);
  g.height_diff = getHeightDiff(nodes, nd1, nd2);
//  g.distance = 1;

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

void truncate(Graph &graph, map<long long, Position>&nodes, long long start_node, long long end_node, double eps){
  map<long long, map<long long, Arc>>::iterator it;
  //GRAPH
  // id -> neighbour_id, arc

  long long i,j,k;
  double ij, ik, jk;

  it = graph.nodes.begin();
  while( it != graph.nodes.end()){

    if(it->second.size() != 2 || it->first == start_node || it->first == end_node){
      ++it;
      continue;
    }

    // j - i - k

    i = it->first;
    j = it->second.begin()->first;
    k = (it->second.begin()++)->first;

    ij = getDistance(nodes, i, j);
    ik = getDistance(nodes, i, k);
    jk = getDistance(nodes, j, k);

    if( abs( (ij+ik) - jk ) < eps){

      graph.nodes[j].erase(i);
      graph.nodes[k].erase(i);

      graph.nodes[j][k].cost = Cost(jk,0);
      graph.nodes[k][j].cost = Cost(jk,0);

      it = graph.nodes.erase(it);
      continue;
    }

    ++it;
  }
}

vector<long long> getNodes(Label* label){
  vector<long long> v;
  Label* label_ptr = label;
  while(label_ptr != nullptr){
    v.push_back(label_ptr->node);
    label_ptr = label_ptr->prev_label;
  }
  return v;
}

string to_json(list<Label*>& labels){

  if(labels.size() == 0){
    cout << "no paths found" << endl;
    return "{}";
  }

  Object obj;
  Array itineraries;
  Object* itinerary;
  Object* criteria;
  Array* sections;
  Object* section;
  Array* nodes;
  vector<long long> nodes_vect;
  obj.add("nb_itineraries", new Integer(labels.size()));
  list<Label*>::iterator it;
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

void init_graph(Graph& myGraph, map<long long,Position> & nodes){
  ifstream graph_file, nodes_file;
  graph_file.open("../Data/data/graph.gr");
  nodes_file.open("../Data/data/talence_nodes.txt");
  string s;
  int nb_arcs;
  graph_file >> s;
  graph_file >> s;
  graph_file >> s;
  graph_file >> nb_arcs;

  long long nd1, nd2;
  double cost;
  double alt;

  for( int i=0; i<nb_arcs; ++i){
    graph_file >> s;
    graph_file >> nd1;
    graph_file >> nd2;
    graph_file >> cost;
//    graph_file >> alt;
    myGraph.nodes[nd1][nd2].cost = Cost(cost,0);
  }

  long long nb_nodes, id;

  nodes_file >> nb_nodes;
  for(long long i=0; i < nb_nodes; ++i){
    nodes_file >> id;
    nodes[id] = Position();
    nodes_file >> nodes[id].lat;
    nodes_file >> nodes[id].lon;
    nodes_file >> nodes[id].alt;
  }

}

void init_graph_complete(Graph& myGraph, map<long long,Position> & nodes, string graph_file_str, string nodes_file_str){

  ifstream graph_file, nodes_file;
  graph_file.open(graph_file_str);
  nodes_file.open(nodes_file_str);

  string s;
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



int main(int argc, char* argv[]){

  // string graph_file(argv[1]);
  // string nodes_file(argv[2]);
  // string in_file(argv[3]);
  // string out_file(argv[4]);



  string graph_file("data/graphWalk.cr");
  string nodes_file("data/nodes.co");
  string in_file("data/userInput.json");
  string out_file("data/output.json");

  Graph myGraph;

  map<long long,vector<long long>> graph;
  map<long long,Position> nodes;

//  init_graph(myGraph, nodes);
  init_graph_complete(myGraph, nodes, graph_file, nodes_file);

  time_t init_time = time(NULL);

  Object* obj = (Object*) jsonParseFile(in_file);

  Real* n1 = (Real*) obj->obj["start"];
  Real* n2 = (Real*) obj->obj["dest"];


//   long long start_node = 2292387593; //yellow
//   long long end_node = 272268867;    //orange
// //  orange 272268867
// //  yellow 2292387593
// //  red 2518636193

 long long start_node = n1->val;
 long long end_node = n2->val;

  // Object* obj = (Object*) jsonParseFile(in_file);
  // Object* obj1 = (Object*) obj->obj["start"];
  // Object* obj2 = (Object*) obj->obj["dest"];
  //
  // Real* n1 = (Real*) obj1->obj["latitude"];
  // Real* n2 = (Real*) obj1->obj["longitude"];
  //
  // Real* n3 = (Real*) obj2->obj["latitude"];
  // Real* n4 = (Real*) obj2->obj["longitude"];
  //
  // long long start_node = getNearest(nodes,n2->val, n1->val);
  // long long end_node = getNearest(nodes, n4->val, n3->val);

//  cout << start_node << " " << end_node << endl;

  //cout << "graph size before " << myGraph.nodes.size() << endl;

  //truncate(myGraph, nodes, start_node, end_node, 1);

  //cout << "graph size after " << myGraph.nodes.size() << endl;

//  initGraph(graph, nodes,"data/graph.txt", "data/nodes.txt", "data/graph.dot");

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
    //choosing a node from open
    node_it = getNondomNode(open);

#ifdef SIZE
    cout << closed.size() << " " << open.size() << " " << (*node_it)->g.distance << endl;
#endif

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
//    vector<long long>& neighbours = graph[current_label->node];             //==============================================================

      map<long long, Arc>& neighbours_arcs = myGraph.nodes[current_label->node];

#ifdef DEBUG
    cout << " - path expansion for " << neighbours_arcs.size() << " neighbours" << endl;
#endif

//    for(unsigned long long i = 0; i<neighbours.size(); ++i){
      for(map<long long, Arc>::iterator it= neighbours_arcs.begin(); it != neighbours_arcs.end() ; ++it){
//      m = neighbours[i];
        m = it->first;

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
        Cost cost_m(0,0);
  //      cost_m = current_label->g  + getCost(nodes, current_label->node, m);  //===============================================================
        cost_m = current_label->g  + neighbours_arcs[m].cost;
        Cost heuristic_m(0,0);

        heuristic_m = getCost(nodes,m,end_node);                              //===============================================================

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

//   list<Label*>::iterator it = best_labels.begin();
// //  ++it;
//   Label* label_ptr = *it;
//
//   while(label_ptr != nullptr){
//     cout << label_ptr->node << " -- " ;
//     label_ptr = label_ptr -> prev_label;
//   }
//   cout << endl;
//
//   cout << best_labels.size() << endl;

  cout << "time elapsed: " << time(NULL) - init_time << "s" << endl;

  ofstream o_file;
  o_file.open(out_file);

  o_file << to_json(best_labels) << endl;
  o_file.close();

  //cout << to_json(best_labels) << endl;

  //cout << best_labels.size() << endl;

  return 0;
}
