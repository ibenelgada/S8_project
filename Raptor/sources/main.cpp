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

#define INF UINT_MAX

using namespace std;

int main()
{

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

    map <int, Bag > bags[total_nb_stops];  //array of multilabel lists
                                                      //t[i]: list of earliest arrival times at stop i for each round

    //vector <unsigned int> t_min(total_nb_stops, INF);     //array for earliest known arrival times at each stop
                                            //used for local pruning

    vector <bool> marked(total_nb_stops,false);  //array for marking stops
//
//    marked could be also like Q an empty array where stops to process are added
//


    vector <int> Q(total_nb_stops, -1);                //array of routes where Q[r] = p means that
    //Q.reserve(total_nb_stops);     //route r should be processed starting with stop p

    int k = 0;                              //Round number

    /* INITIALIZING VARIABLES */

    //earliest arrival time for the source stop is set to TO (departure time)
    //t_min[ps_id] = to;
    bags[ps_id][0] = to;

    //mark the source stop ps
    marked[ps_id] = true;

    /* Some temporary variables */
    int h, p, r, pp, pi, p_pos, pi_pos, trip;
    unsigned int t_arr,p_t;
    string tmp_s;
    bool marked_any;

    while(true){

        //increment round number
        k++;

        cout << endl << "------------------------------------" << endl;
        cout << "              ROUND " << k << endl;
        cout << "------------------------------------" << endl << endl;

        //Clear Q
        Q.clear();


        //Loop through marked stops only
        for(p=0; p<myNetwork.nb_stops(); p++){
            if(!marked[p])
                continue;

            //Loop through routes serving the current marked stop p
            for(int j=0; j<myNetwork.route_nb_stops(); j++){
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
