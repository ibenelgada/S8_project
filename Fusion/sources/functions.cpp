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
#include "Bag.h"
#include "Bags.h"

#include "Network.h"
#include "Json.h"

//#define ELAPSED_TIME      //uncomment to print the elapsed time of the algorithm


using namespace std;

double time_str_to_n(string s){
  double h,m;
  string h_s, m_s;

  h_s = s.substr(0, s.find(":"));
  m_s = s.substr(s.find(":")+1);

  h = atof(h_s.c_str());
  m = atof(m_s.c_str());
  return h*60 + m;
}

string fusion(Graph& myGraph, std::map<long long, Position>& nodes, Network& myNetwork, long long start_node, long long end_node, double start_time){

  map<long long, Route> &routes = myNetwork.routes;
  map<long long, Stop> &stops = myNetwork.stops;

  long long nb_routes = myNetwork.nb_routes();
  long long total_nb_stops = myNetwork.nb_stops();

  Bag best_labels;

  map<int, Bag> best_labels_k;

   list<Label*> list_tmp = Namoa(myGraph, nodes, start_node, end_node, start_time);

   best_labels = Bag(list_tmp);
   best_labels_k[0] = best_labels;

  long long distance = 2000;
  vector<long long> source_stops;
  getStops(source_stops, nodes, stops, start_node, distance);

//  cout << "found " << source_stops.size() << " stops nearby"<< endl;


   // cout << "============ source_stops ============" << endl;
   // for(unsigned int i=0; i<source_stops.size(); ++i){
   //   cout << " " << source_stops[i];
   // }
   // cout << endl;

   /* DEFINING VARIABLES  */
   //
   // long long pt_id = 365596520;
   // unsigned long long to = 0;  //departure time



   Bags B;                   //vector of nb_stops maps B(p) where B(p)
                             //is a map of bags B(p,k), round k is the key
                             // usage: B(p,k)

   map <long long,Bag> bags_star;

   map<long long, bool> marked;  //map for marking stops

 //
 //    marked could be also an empty array where stops to process are added
 //

   map<long long, long long> Q;   //map of routes where Q[r] = p means that
   //Q.reserve(total_nb_stops);     //route r should be processed starting with stop p

   int k = 0;                              //Round number

   /* Some temporary variables */
   long long i,p, r, pp, pi, p_pos, pi_pos;
   string tmp_s;
   bool marked_any;
   Bag Br;
   Bag::const_iterator c_it;
   Bag::iterator it;
   Label l_tmp;
   vector<long long> trips;
   map<long long, long long>::iterator sr_it;
   map<long long, Stop>::iterator s_it;
   map<long long, Route>::iterator r_it;

   list<Label*> open_labels;
   Bag bag_tmp;
   /* INITIALIZING VARIABLES */

   for(unsigned int i=0; i < source_stops.size(); ++i){
     B(source_stops[i], 0).bag = Namoa(myGraph, nodes, start_node, source_stops[i], start_time);
     bags_star[source_stops[i]] = B(source_stops[i], 0);
     marked[source_stops[i]] = true;
//     cout << source_stops[i] << " marked" << endl;
   }

   // Label l;
   // l.node = ps_id;
   // l.g.time = to;
   // l.g.distance = 0.0;
   // B(ps_id,0).push_nondom(&l);
   // bags_star[ps_id].push_nondom(&l);

   //mark the source stop ps
 //  marked[ps_id] = true;

   while(true){

       //increment round number
       k++;

       // cout << endl << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
       // cout << "              ROUND " << k << endl;
       // cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl << endl;
       //
       marked_any = false;

       //Clear Q
     //  Q.assign(nb_routes,-1);
       Q.clear();
//       cout << "Clearing Q" << endl;

       //Loop through marked stops only
         for(s_it=stops.begin(); s_it!=stops.end(); ++s_it){

           p = s_it->first;  //stop id
           if(marked.find(p)==marked.end() || marked[p]==false)
             continue;

           //Loop through routes serving the current marked stop p
           for(sr_it=stops[p].routes.begin(); sr_it != stops[p].routes.end(); ++sr_it){
               r = sr_it->first;        //route_id

               if(Q.find(r)!=Q.end()){           //if route r is in Q
                   pp = Q[r];
                   //if pp comes before p in route r
                   if(myNetwork.route_pos(r,p) >= myNetwork.route_pos(r,pp))
                       continue;
               }
               Q[r] = p;       //add (r,p) to Q
           }
           marked[p] = false;  //unmark p
       }  //fill Q

       //DISPLAY routes
  //     cout << "Routes (r,p) in Q are : " << endl;

       for(r_it=routes.begin(); r_it!=routes.end(); ++r_it){
         i = r_it->first;
         if(Q.find(i)!= Q.end())
          ;// cout << "(" << i << "," << Q[i] << ")" << endl;
       }
      // cout << endl;


       //Processing each route in Q

        for(r_it=routes.begin(); r_it!=routes.end(); ++r_it){
          r = r_it->first;
          if(Q.find(r) == Q.end())
             continue;

      //     cout << "----------------------------------------------------------------------------------------------------processing route " << r << " starting with stop " << Q[r] << endl;

           p = Q[r];
           p_pos = myNetwork.route_pos(r,p);

           Br.clear();

           //foreach stop pi in r beginning with stop p
           for(long long j=p_pos; j<routes[r].nb_stops; ++j){

               pi = routes[r].stops[j];
      //         cout << "---------------------------------------------------------------------------Stop " << pi << endl;
               pi_pos = j;

               // cout << "Br is:" << endl << Br << endl;
               // cout << "B* is:" << endl << bags_star[pi] << endl;
               //
               // cout << "-------------------------traversing Br labels" << endl;
               for(it=Br.begin(); it!=Br.end(); ++it){

                 //--------------------------------------------------------
                 Label& l_ref = **it;

                 l_ref.g.time = routes[r].trips[l_ref.info.trip][pi_pos];
                 l_ref.g.price = l_ref.prev_label->g.price + myNetwork.getCost(r, l_ref.info.trip, l_ref.prev_label->node, pi);
                 l_ref.node = pi;
//                 cout << l_ref << endl;

                 if( l_ref <= Bag(best_labels) && bags_star[pi].push_nondom(&l_ref) ){

                     B(pi,k).push_nondom(&l_ref);
                     marked[pi] = true;
                     marked_any = true;
          //           cout << "adding Label to bag and marking " << pi << endl;

                 }else{
                   ;
  //                   cout << "Label not added" << endl;
                 }
               } //Br Label loop

               // cout << "-------------------------end of Br" << endl;
               // cout << "B* of stop " << pi << " now is:" << endl;
               // cout << bags_star[pi] << endl;
               //
               // cout << "-------------------------traversing B*" << endl;

               for(c_it=bags_star[pi].cbegin(); c_it!=bags_star[pi].cend(); ++c_it){
          //       cout << **c_it << endl;

                 trips.clear();
                 myNetwork.get_trips(r, pi, (*c_it)->g.time, trips);

            //     cout << "trips is of size " << trips.size() << endl;

                 for(unsigned long long i=0; i<trips.size(); ++i){

            //       cout << "trip " << trips[i] << ":" << endl;

                   if(trips[i] < 0){
            //         cout << "Label not added to Br" << endl;
                     break;
                   }

                   l_tmp.fill(*c_it, r, trips[i]);
                   // cout << "calculated label is:" << endl;
                   // cout << l_tmp << endl;
                   Label* label_k;

                   if(Br.push_nondom(&l_tmp, label_k)){
                     ++label_k->g.k;
            //         cout << "Label added to Br" << endl;
                   }
                   else
              ;//       cout << "Label not added to Br" << endl;
                 } // trips

               } //add B*(pi) to Br
               //
               // cout << "-------------------------end of B*" << endl;
               // cout << "Br now is:" << endl;
               // cout << Br << endl;




             } //route stops loop

           } //route loop

           open_labels.clear();

           //If no stops are marked, end
           if(!marked_any)
             break;

           for(s_it=stops.begin(); s_it!=stops.end(); ++s_it){

             p = s_it->first;  //stop id
               if(marked.find(p) == marked.end() || marked[p] == false)
                 continue;

             for(it = B(p,k).begin(); it != B(p,k).end(); ++it){
               open_labels.push_back(*it);
             }

           }

           truncate(nodes, open_labels, end_node, 2000);

    //       cout << "----------------------------------------------------------------------------------------------------Calculating path to dest from " << open_labels.size() << " stops"<< endl;


           list_tmp = Namoa(myGraph, nodes, end_node, open_labels);


           bag_tmp = Bag(list_tmp);
           //
           // cout << "-------------------------paths found:" << endl;
           // cout << bag_tmp;
           // cout << "-------------------------adding paths" << endl;
           // cout << "best labels before" << endl;
           // cout << best_labels;

           for(it = bag_tmp.begin(); it != bag_tmp.end(); ++it){
    //         cout << **it << endl;
             if(best_labels.push_nondom(*it))
             ;
              // cout << "Label added" << endl;
           }
           //
           // cout << "best labels after" << endl;
           // cout << best_labels;

   } //rounds

   // cout << endl << "------------------------------------" << endl;
   // cout << "         End of algorithm"<< endl;
   // cout << "------------------------------------" << endl << endl;
   //
   //
   // cout << best_labels << endl;

   // for(it = best_labels.begin(); it!=best_labels.end(); ++it){
   //   cout << (*it)->to_path() << endl;
   // }


     string result;
     result = to_json(best_labels.bag, myNetwork, start_node, end_node, start_time);

  //   for(Bag::iterator itt = best_labels.begin(); itt!=best_labels.end(); ++itt)
  //    cout << (*itt)->to_path() << endl;

     return result;
 }

void getStops(vector<long long>& v, map<long long,Position> &nodes, map<long long, Stop>& stops, long long start_node, long long d){

  map<long long, Stop>::iterator it;
  long long stop_id;
  double tmp_d;
  for (it = stops.begin(); it != stops.end(); ++it) {
    stop_id = it->first;
    tmp_d = getDistance(nodes, stop_id, start_node);
    if(tmp_d <= d)
      v.push_back(stop_id);

  }
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

  return 6371000 * c;

}

double getHeightDiff(std::map<long long,Position> &nodes, long long nd1, long long nd2){
  return nodes[nd2].alt - nodes[nd1].alt > 0 ? nodes[nd2].alt - nodes[nd1].alt : 0;
}

Cost getCost(std::map<long long,Position> &nodes, long long nd1, long long nd2){
  Cost g;
  g.distance = getDistance(nodes, nd1, nd2);
//  g.height_diff = getHeightDiff(nodes, nd1, nd2);
  g.time = g.distance/60;

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

void filter(std::list<std::pair<Label*,Cost>>& open_all, Cost c){

    std::list<std::pair<Label*,Cost>>::iterator it;
    it = open_all.begin();
    while(it != open_all.end()){
        if(c < it->second){
            it = open_all.erase(it);
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

  std::list<std::pair<Label*,Cost>> open_copy;
  std::list<std::pair<Label*,Cost>>::iterator it;


  for(it = open_all.begin(); it != open_all.end(); ++it){
    if(it->second.distance != open_all.begin()->second.distance)
      break;
    open_copy.push_back(*it);
  }

  if(open_copy.size() == 1)
    return open_all.begin();


  for(it = open_copy.begin(); it!=open_copy.end();){
    if(isNondom(open_copy,it))
      break;
    filter(open_copy, it->second);
    it = open_copy.erase(it);
  }

  if(it==open_copy.end()){
    return open_all.begin();
  }

  for(std::list<std::pair<Label*,Cost>>::iterator ito = open_all.begin(); ito!=open_all.end(); ++ito){
    if(*it == *ito){
      return ito;
    }
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

vector<vector<Label* >> getMNodes(Label * label){

    vector<vector <Label*>> mat;
    Label* l_tmp = label;

    long long route = l_tmp->info.route;
    vector<Label*> v;

    while(l_tmp != nullptr){
      v.push_back(l_tmp);
      l_tmp = l_tmp->prev_label;

      if(l_tmp == NULL){
        mat.push_back(v);
        break;
      }

      if(l_tmp->info.route != route){
        route = l_tmp->info.route;
        v.push_back(l_tmp);
        mat.push_back(v);
        v.clear();
      }

    }

    return mat;

}

std::string to_json(std::list<Label*>& labels, Network& net, long long start_node, long long end_node, double start_time){

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
  vector<Label*> nodes_vect;
  vector<vector<Label*>> nodes_matrix;

  obj.add("nb_itineraries", new Integer(labels.size()));
  obj.add("start", new Integer(start_node));
  obj.add("dest", new Integer(end_node));
  std::list<Label*>::iterator it;
  Label* label;

  for(it = labels.begin(); it != labels.end(); ++it){

    itinerary = new Object();
    label = *it;
    criteria = new Object();

    criteria->add("distance", new Integer(label->g.distance));
    criteria->add("time", new Integer(label->g.time - start_time));
    criteria->add("price", new Real(0));
    criteria->add("height", new Integer(label->g.height_diff));
    criteria->add("connections", new Integer(label->g.k));
    criteria->add("co2" , new Integer(0));
    criteria->add("effort", new Integer(0));

    itinerary->add("criteria", criteria);

    sections = new Array();

    for(int i=0; i<1; ++i){


      nodes_matrix.clear();

      nodes_matrix = getMNodes(label);

      for(unsigned int j=0; j < nodes_matrix.size(); ++j){
        section = new Object();
        nodes = new Array();
        nodes_vect = nodes_matrix[j];
        int start_t;
        int end_t;
        if(nodes_vect[0]->info.route < 0){
          for(unsigned int i=0; i < nodes_vect.size(); ++i){
            nodes->arr.push_back(new Integer(nodes_vect[i]->node));
          }
          start_t = nodes_vect.back()->g.time;
          end_t = nodes_vect.front()->g.time;
        }else{
          Route& route = net.routes[nodes_vect[0]->info.route];
          int trip = nodes_vect[0]->info.trip;
          long long start_n = nodes_vect.back()->node;
          long long end_n = nodes_vect.front()->node;
          unsigned int a;
          for( a = 0; a < route.stops.size(); ++a)
            if(route.stops[a] == end_n)
              break;

          while(route.stops[a] != start_n){
            nodes->arr.push_back(new Integer(route.stops[a]));
            --a;
          }
          nodes->arr.push_back(new Integer(start_n));
          start_t = route.trips[trip][net.route_pos(route.id, start_n)];
          end_t = nodes_vect.front()->g.time;
        }
        section->add("nodes", nodes);
        section->add("start", new Integer(start_t));
        section->add("end", new Integer(end_t));
        section->add("idTransport", new Integer(nodes_vect[0]->info.route));
        section->add("public", new Boolean(nodes_vect.front()->info.route != -1));
        section->add("type", new String("walking"));

        sections->arr.push_back(section);
      }
    }

    itinerary->add("sections", sections);

    itineraries.arr.push_back(itinerary);
  }

  obj.add("itineraries", &itineraries);

  return obj.toString();
}

void truncate(map<long long,Position> &nodes, list<Label*> &open_labels, long long end_node, double d){

  list<Label*>::iterator it;
  long long n;

  for(it=open_labels.begin(); it!=open_labels.end(); ++it){
    n = (*it)->node;
    if(getDistance(nodes, n, end_node) > d)
      it = open_labels.erase(it);
  }
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
    Cost c;
    c.distance = dist;
    c.time = time/60.0;
    myGraph.nodes[nd1][nd2].cost = c;


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


void get_start_end(const std::string& in_file, long long& start_node, long long& end_node, double& start_time){

    Object* obj = (Object*) jsonParseFile(in_file);

    Real* n1 = (Real*) obj->obj["start"];
    Real* n2 = (Real*) obj->obj["dest"];

    String* t = (String*) obj->obj["startTime"];

   start_node = n1->val;
   end_node = n2->val;
   start_time = time_str_to_n(t->val);

}

void push_ordered(list<pair<Label*,Cost>> &open_all, Label* lb, Cost eval){

  list<pair<Label*,Cost>>::iterator node_it;
  pair<Label*, Cost> p(lb,eval);


  for(node_it = open_all.begin(); node_it != open_all.end(); ++node_it){
    if(node_it->second.distance >= eval.distance){
      open_all.insert(node_it, p);
      break;
    }

  }
  if(node_it == open_all.end())
    open_all.push_back(p);

}

std::list<Label*> Namoa(Graph& myGraph, std::map<long long, Position>& nodes, long long start_node, long long end_node, double start_time){
  list<pair<Label*,Cost>> open_all;
  Label* label_tmp = new Label(start_node);
  label_tmp->g.time = start_time;
  open_all.push_back(pair<Label*,Cost>(label_tmp,Cost()));
  return Namoa(myGraph, nodes, end_node, open_all);
}

std::list<Label*> Namoa(Graph& myGraph, std::map<long long, Position>& nodes, long long end_node, std::list<Label*>& labels){
  list<pair<Label*,Cost>> open_all;
  list<Label*>::iterator it;

  for(it = labels.begin(); it != labels.end(); ++it){
    open_all.push_back(pair<Label*,Cost>(*it,Cost()));
  }
  return Namoa(myGraph, nodes, end_node, open_all);
}

std::list<Label*> Namoa(Graph& myGraph, std::map<long long, Position>& nodes, long long end_node, list<pair<Label*,Cost>> open_all){

    double h_factor = 1.0;


    Label* label_tmp;
    map<long long, list<Label*>> open;
    map<long long, list<Label*>> closed;
    list<Label*> best_labels;

    list<pair<Label*,Cost>>::iterator node_it;
    Label* current_label;
    long long m;
    Cost eval_m;
    Cost cost_m;
    Cost heuristic_m;
    Label* new_label = NULL;

    for(node_it = open_all.begin(); node_it != open_all.end(); ++node_it){
      node_it->second = getCost(nodes, node_it->first->node, end_node);
      open[node_it->first->node].push_back(node_it->first);
    }


  #ifdef ELAPSED_TIME
    chrono::microseconds ms = chrono::duration_cast< chrono::microseconds >(chrono::system_clock::now().time_since_epoch());
    chrono::microseconds ms1;
  #endif

  int counter(0);

    while(!open_all.empty()){
      ++counter;
      //choosing a node from open
      node_it = getNondomNode(open_all);



  //    cout << counter << endl;
  //    std::cout << (*node_it).first->node << std::endl;

    //  std::cout << counter << " "  << open_all.size() << " "<< (*node_it).first->g.distance << " " << (*node_it).second.distance - (*node_it).first->g.distance  << std::endl;

      // string str;
      // cin >> str;

      //moving node from open to closed
      current_label = node_it->first;
      open_all.erase(node_it);
      open[current_label->node].remove(current_label);
      closed[current_label->node].push_back(current_label);


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

            push_ordered(open_all, new_label, eval_m);
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

            push_ordered(open_all, new_label, eval_m);
            open[m].push_back(new_label);


        } //if

      } //for

    } //while


  #ifdef ELAPSED_TIME
    ms1 = chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch() );
    cout << "time elapsed " << (ms1.count() - ms.count()) / 1000000.0 << "s" << endl;
  #endif

    return best_labels;
}
