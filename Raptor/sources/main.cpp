#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <map>
#include <climits>
#include <fstream>

#include "Stop.h"
#include "Route.h"
#include "Network.h"
#include "Position.h"
#include "Label.h"
#include "Bag.h"
#include <cmath>

#define INF UINT_MAX

using namespace std;



int route_pos(const vector<Stop> &stops, int s, int r){
    /*
    Gives the position of a stop s in a route r,
    used to check if one stop comes before another stop
    */
    for(int i=0; i<stops[s].nb_routes; i++){
        if(stops[s].routes[i] == r)
            return stops[s].route_pos[i];
    }
    cout << "Route " << r << " not found in stop " << s << endl;
    return -1;
}

int et(const vector<Route> &routes, const vector<Stop> &stops, int r, int pi, unsigned int t_pi){
  /*
  Returns the earliest trip that can be taken from the stop pi in the route
  r at the departure time t_pi
  */
  int pi_pos = route_pos(stops, pi, r);
  unsigned int t_arr;

  for(int t=0; t<routes[r].nb_trips; t++){
    t_arr = routes[r].trips[t*routes[r].nb_stops + pi_pos];
    if(t_pi <= t_arr)
    return t;
  }
  return -1;
}

void get_trips(const vector<Route> &routes, const vector<Stop> &stops, int r, int pi, unsigned int t_pi, vector<int> &trips){

  int t;
  t = et(routes, stops, r, pi, t_pi);

  for(int i=t; i<routes[r].nb_trips; i++){
    trips.push_back(i);
  }

}

float getCost(const vector<Route> &routes, const vector<Stop> &stops, int r, int trip, int p1, int p2){
  float price = 0.5;
  return price * abs((float)(route_pos(stops, p1, r) - route_pos(stops, p2, r)));
}


bool compare(const Label& l, const Bag& b){
  list<Label>::const_iterator it;
  for(it = b.bag.begin(); it != b.bag.end(); ++it){
    if( *it < l )
      return false;
  }
  return true;
}

bool add_nondom(const Label& l, Bag& b){
  list<Label>::iterator it;
  it = b.bag.begin();
  while(it != b.bag.end()){
    if( *it < l || *it == l )
      return false;
    if( *it > l)
      it = b.bag.erase(it);
    else
      ++it;
  }
  b.bag.push_back(l);
  return true;
}


int main(){

    Network myNetwork("data/routes.txt", "data/pathways.txt");

    vector <Route> &routes = myNetwork.routes;
    vector <Stop> &stops = myNetwork.stops;

    int nb_routes = routes.size();
    int total_nb_stops = stops.size();
/* ----------------------------------------------------------------------------------------------------
   Algorithm implementation : RAPTOR
*/


    /* DEFINING VARIABLES  */

    int ps_id = 0;        //source stop
    int pt_id = 14;       //target stop
    unsigned int to = 0;  //departure time

    map <int, Bag > bags[total_nb_stops];  //array of nb_stops maps B(p) where B(p)
                                           //is a map of bags B(k,p), round k is the key
                                           // usage: bags[p][k]

    //vector <unsigned int> t_min(total_nb_stops, INF);     //array for earliest known arrival times at each stop
                                            //used for local pruning

    Bag bags_star[total_nb_stops];

    vector <bool> marked(total_nb_stops,false);  //array for marking stops

//
//    marked could be also like Q an empty array where stops to process are added
//

    vector <int> Q(nb_routes, -1);                //array of routes where Q[r] = p means that
    //Q.reserve(total_nb_stops);     //route r should be processed starting with stop p

    int k = 0;                              //Round number

    /* Some temporary variables */
    int p, r, pp, pi, p_pos, pi_pos;
    string tmp_s;
    bool marked_any;
    Bag Br;
    list<Label>::iterator it;
    Label l_tmp;
    vector<int> trips;

    /* INITIALIZING VARIABLES */

    //earliest arrival time for the source stop is set to TO (departure time)
    //t_min[ps_id] = to;
    Label l;
    l.time = to;
    l.price = 0.0;

    bags[ps_id][0] = Bag();
    bags[ps_id][0].bag.push_back(l);
    bags_star[ps_id].bag.push_back(l);
    //mark the source stop ps
    marked[ps_id] = true;


    while(true){

        //increment round number
        k++;

        cout << endl << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "              ROUND " << k << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl << endl;

        marked_any = false;

        //Clear Q
        for(unsigned int i=0; i< Q.size(); ++i){
          Q[i] = -1;
        }
        cout << "Clearing Q" << endl;


        //Loop through marked stops only
        for(p=0; p<myNetwork.nb_stops(); ++p){
            if(!marked[p])
                continue;

            //Loop through routes serving the current marked stop p
            for(int j=0; j<myNetwork.stop_nb_routes(p); j++){
                r = stops[p].routes[j];   //route

                if(Q[r] != -1){           //if route r is in Q
                    pp = Q[r];
                    //if pp comes before p in route r
                    if(route_pos(stops,p,r) >= route_pos(stops,pp,r))
                        continue;
                }
                Q[r] = p;       //add (r,p) to Q
            }
            marked[p] = false;  //unmark p
        }  //fill Q

        //DISPLAY routes
        cout << "Routes (r,p) in Q are : " << endl;
        for(int i=0; i < nb_routes; ++i){
            if(Q[i]!= -1)
              cout << "(" << i << "," << Q[i] << ")" << endl;
        }
        cout << endl;


        //Processing each route in Q
        for(r=0; r<nb_routes; ++r){
            if(Q[r] == -1)
                continue;

            cout << "----------------------------------------------------------------------------------------------------processing route " << r << " starting with stop " << Q[r] << endl;

            p = Q[r];
            p_pos = route_pos(stops, p, r);

            Br.bag.clear();

            //foreach stop pi in r beginning with stop p
            for(int j=p_pos; j<routes[r].nb_stops; ++j){

                pi = routes[r].stops[j];
                cout << "---------------------------------------------------------------------------Stop " << pi << endl;
                pi_pos = j;

                cout << "Br is:" << endl << Br << endl;
                cout << "B* is:" << endl << bags_star[pi] << endl;

                cout << "-------------------------traversing Br labels" << endl;
                for(it=Br.bag.begin(); it!=Br.bag.end(); ++it){



                  //--------------------------------------------------------
                //  cin >> s;
                  Label& l_ref = *it;

                  l_ref.time = routes[r].trips[l_ref.trip * routes[r].nb_stops + pi_pos];
                  l_ref.price = l_ref.prev_label->price + getCost(routes, stops, r, l_ref.trip, l_ref.hop_stop, pi);

                  cout << l_ref << endl;

                  if( compare(l_ref, bags_star[pt_id]) && add_nondom(l_ref, bags_star[pi]) ){
                    if(bags[pi].find(k) == bags[pi].end()){
                      cout << "Creating Bag B of stop " << pi << " and round " << k << endl;
                      bags[pi][k] = Bag();
                    }
                      add_nondom(l_ref, bags[pi][k]);
                      marked[pi] = true;
                      marked_any =true;
                      cout << "adding Label to bag and marking " << pi << endl;

                  }else{
                      cout << "Label not added" << endl;
                  }
                } //Br Label loop

                cout << "-------------------------end of Br" << endl;
                cout << "B* of stop " << pi << " now is:" << endl;
                cout << bags_star[pi] << endl;

                cout << "-------------------------traversing B*" << endl;


                for(it=bags_star[pi].bag.begin(); it!=bags_star[pi].bag.end(); ++it){
                  cout << *it << endl;

                  trips.clear();
                  get_trips(routes, stops, r, pi, it->time, trips);

                  cout << "trips is of size " << trips.size() << endl;

                  for(unsigned int i=0; i<trips.size(); ++i){
                    cout << "trip " << trips[i] << ":" << endl;
                    l_tmp = *it;
                    l_tmp.prev_label = it;
                    l_tmp.trip = trips[i];
                    l_tmp.route = r;
                    l_tmp.hop_stop = pi;
                    if(add_nondom(l_tmp, Br))
                      cout << "Label added to Br" << endl;
                    else
                      cout << "Label not added to Br" << endl;

                  } // trips

                } //add B*(pi) to Br

                cout << "-------------------------end of B*" << endl;
                cout << "Br now is:" << endl;
                cout << Br << endl;


              } //route stops loop

            } //route loop


        //If no stops are marked, end
        if(!marked_any)
            break;

    } //rounds

    cout << endl << "------------------------------------" << endl;
    cout << "         End of algorithm"<< endl;
    cout << "------------------------------------" << endl << endl;


    cout << bags_star[pt_id] << endl;

    return 0;
}
