#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <climits>
#include <fstream>

#define INF UINT_MAX

using namespace std;



class position{
    double lat;
    double lon;
};

class Stop{

public:

    int id;
    position pos;
    int nb_routes;
    int nb_transfers;
    vector <int> transfer_to;
    vector <int> transfer_time;
    vector <int> routes;
    vector <int> route_pos;

    void set(int p_id, int p_nb_routes, int p_nb_transfers){
      id = p_id;
      nb_routes = p_nb_routes;
      nb_transfers = p_nb_transfers;
    }

    void add_transfert(int to, unsigned int time){
      transfer_to.push_back(to);
      transfer_time.push_back(time);
    }

    void add_route(){

    }
};

class Route{

public:

    int id;
    int nb_stops;
    int nb_trips;
    vector <int> stops;
    vector <int> trips;

    void set(int p_id, int p_nb_stops, int p_nb_trips){
      id = p_id;
      nb_stops = p_nb_stops;
      nb_trips = p_nb_trips;
    }

};

class Network{
public:
  vector <Route> routes;
  vector <Stop> stops;

public:

  Network (const string& froutes, const string& fpaths ){
    ifstream sroutes, spaths;

    sroutes.open(froutes.c_str());
    spaths.open(fpaths.c_str());

    init(sroutes, spaths);

    sroutes.close();
    spaths.close();
  }

  Network(istream& sroutes, istream& spaths){
    init(sroutes, spaths);
  }

  void init(istream& sroutes, istream& spaths){
    int nb_routes, total_nb_stops;

    int s_id, nb_stop_routes, nb_transfers;

    unsigned int transfer_to, transfer_time;

    int r_id, nb_stops, nb_trips;

    int stop, time;

    sroutes >> nb_routes;
    sroutes >> total_nb_stops;

    routes.resize(nb_routes);
    stops.resize(total_nb_stops);

    for(int i=0; i<total_nb_stops; i++){
      spaths >> s_id;
      spaths >> nb_stop_routes;
      spaths >> nb_transfers;

      stops[i].set(s_id, nb_stop_routes, nb_transfers);

      for(int j=0; j<nb_transfers; j++){
        spaths >> transfer_to;
        spaths >> transfer_time;

        stops[i].add_transfert(transfer_to, transfer_time);
      }

    }

    for(int i=0; i<nb_routes; i++){
      sroutes >> r_id;
      sroutes >> nb_stops;
      sroutes >> nb_trips;

      routes[i].set(r_id, nb_stops, nb_trips);

      for(int j=0; j<nb_stops; j++){
        sroutes >> stop;
        routes[i].stops.push_back(stop);
        stops[stop].routes.push_back(routes[i].id);
        stops[stop].route_pos.push_back(j);
      }

      for(int j=0; j<nb_trips; j++){
        for(int k=0; k<nb_stops; k++){
          sroutes >> time;
          routes[i].trips.push_back(time);
        }
      }

    }



  }

};

void showlist(list <unsigned int> g)
{
    list <unsigned int> :: iterator it;
    for(it = g.begin(); it != g.end(); ++it){
        if(*it == INF)
          cout << '\t' << "INF";
        else
          cout << '\t' << *it;
      }
    cout << '\n';
}


int route_pos(vector<Stop> stops, int s, int r){
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

int et(vector<Route> routes, vector<Stop> stops, int r, int pi, unsigned int t_pi){
    /*
    Returns the earliest trip that can be taken from the stop pi in the route
    r at the departure time t_pi
    */
    int pi_pos = route_pos(stops, pi, r);
    unsigned int t_arr;

    for(int t=0; t<routes[r].nb_trips; t++){
        t_arr = routes[r].trips[t*routes[r].nb_stops + pi_pos];
        if(t_pi < t_arr)
            return t;
    }
    return -1;
}

int main()
{


    Network myNetwork("data/routes.txt", "data/pathways.txt");

    vector <Route> routes = myNetwork.routes;
    vector <Stop> stops = myNetwork.stops;

    int nb_routes = routes.size();
    int total_nb_stops = stops.size();
/* ----------------------------------------------------------------------------------------------------
   Algorithm implementation : RAPTOR
*/


    /* DEFINING VARIABLES  */

    int ps_id = 0;        //source stop
    int pt_id = 14;       //target stop
    unsigned int to = 0;  //departure time

    list <unsigned int> t[total_nb_stops];  //array of multilabel lists
                                            //t[i]: list of earliest arrival times at stop i

    vector <unsigned int> t_min(total_nb_stops, INF);     //array for earliest known arrival times at each stop
                                            //used for local pruning

    vector<bool> marked(total_nb_stops,false);  //array for marking stops

    vector <int> Q(nb_routes,-1);                //array of routes where Q[r] = p means that
                                            //route r should be processed starting with stop p

    int k = 0;                              //Round number

    /* INITIALIZING VARIABLES */

    for(int i=0; i<total_nb_stops; i++){    //initialize earliest arrival times with infinity
        t[i].push_back(INF);
    }

    //earliest arrival time for the source stop is set to TO (departure time)
    t_min[ps_id] = to;
    t[ps_id].pop_back();
    t[ps_id].push_back(to);

    //mark the source stop ps
    marked[ps_id] = true;

    /* Some temporary variables */
    int h,p,r,pp,pi,p_pos,pi_pos,trip;
    unsigned int t_arr,p_t;
    string tmp_s;

    while(true){

        //increment round number
        k++;

        cout << endl << "------------------------------------" << endl;
        cout << "              ROUND " << k << endl;
        cout << "------------------------------------" << endl << endl;

        //Clear Q
        for(int i=0; i<nb_routes; i++)
            Q[i] = -1;

        //Loop through marked stops only
        for(p=0; p<total_nb_stops; p++){
            if(!marked[p])
                continue;

            //Loop through routes serving the current marked stop p
            for(int j=0; j<stops[p].nb_routes; j++){
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
        }

        cout << "Routes (r,p) in Q are : " << endl;
        for(int i=0; i < nb_routes; i++){
            if(Q[i]!= -1)
              cout << "(" << i << "," << Q[i] << ")" << endl;
        }
        cout << endl;

        for(int i=0; i<total_nb_stops; i++){
            t[i].push_back(t[i].back());
        }

        //Processing each route in Q
        for(r=0; r<nb_routes; r++){
            if(Q[r] == -1)
                continue;

            cout << "processing route " << r << " starting with stop " << Q[r] << endl;

            p = Q[r];
            p_pos = route_pos(stops, p, r);
            trip = -1;
            t_arr = INF;
            //foreach stop pi in r beginning with stop p
            for(int j=p_pos; j<routes[r].nb_stops; j++){
                pi = routes[r].stops[j];
                cout << "Stop " << pi << endl;

                pi_pos = j;

                if(trip != -1){   //if the trip is defined
                    t_arr = routes[r].trips[trip*routes[r].nb_stops + pi_pos];
                    //improve label if possible
                    if (t_arr < min(t_min[pi],t_min[pt_id])){
                        t[pi].pop_back();
                        t[pi].push_back(t_arr);
                        t_min[pi] = t_arr;
                        marked[pi] = true;
                        cout << pi << " updated with arrival time "  << t[pi].back()<< endl;
                    }
                }

                //ckeck if we can catch an earlier trip
                if(trip == -1 || t[pi].back() < t_arr){
                    trip = et(routes, stops, r, pi, t[pi].back());
                    cout << "trip " << trip << " added"<< endl;
                }

            }
        }

        /* FOOTPATHS */
        //foreach marked stop p
        for(int p=0; p<total_nb_stops; p++){
            if(!marked[p])
                continue;

            //loop through  the stop's transfers
            for(int j=0; j<stops[p].nb_transfers; j++){
                pp = stops[p].transfer_to[j];
                p_t = stops[p].transfer_time[j];

                //Consider footpath if it minimizes arrival time
                if(t[pp].back() > t[p].back() + p_t){
                    t[pp].pop_back();
                    t[pp].push_back(t[p].back() + p_t);
                    marked[pp] = true;
                    cout << pp << " updated with footpath from " << p << " with "  << t[pp].back()<< endl;
                }
            }
        }

        //If no stops are marked, end
        for(h=0; h<total_nb_stops; h++){
            if(marked[h])
                break;
        }

        if(h >= total_nb_stops)
            break;

    }

    cout << endl << "------------------------------------" << endl;
    cout << "         End of algorithm"<< endl;
    cout << "------------------------------------" << endl << endl;

    for(int i=0; i<total_nb_stops; i++){
        cout << "stop " << i << '\t';
        showlist(t[i]);
    }

    cout << "arrived at stop " << pt_id << " at " << t_min[pt_id] << endl;
    return 0;
}
