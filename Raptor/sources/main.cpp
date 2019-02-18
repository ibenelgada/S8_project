#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>

#define INF 4294967295   //2^32 - 1 (biggest unsigned int)

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
    int* transfer_to;
    int* transfer_time;
    int* routes;
    int* route_pos;
};

class Route{

public:

    int id;
    int nb_stops;
    int nb_trips;
    int* stops;
    int* trips;

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


int route_pos(Stop* stops, int s, int r){
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

int et(Route* routes, Stop* stops, int r, int pi, unsigned int t_pi){
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
    FILE* routes_file = fopen("data/routes.txt", "r");
    FILE* pathways_file = fopen("data/pathways.txt", "r");

    int nb_routes;
    int total_nb_stops;

    fscanf(routes_file,"%d %d", &nb_routes, &total_nb_stops);

    Route routes[nb_routes];
    Stop stops[total_nb_stops];

    int tmp;

/* ----------------------------------------------------------------------------------------------------
    Filling the arrays routes and stops with all needed information.
*/

    for(int i=0; i<total_nb_stops; i++){
        fscanf(pathways_file, "%d %d %d", &stops[i].id, &stops[i].nb_routes, &stops[i].nb_transfers);

        stops[i].routes = new int[stops[i].nb_routes];
        stops[i].route_pos = new int[stops[i].nb_routes];

        if(stops[i].nb_transfers > 0){
            stops[i].transfer_to = new int[stops[i].nb_transfers];
            stops[i].transfer_time = new int[stops[i].nb_transfers];
        }

        for(int j=0; j<stops[i].nb_routes; j++)
            stops[i].routes[j] = -1;

        for(int j=0; j<stops[i].nb_transfers; j++){
            fscanf(pathways_file, "%d %d ", &stops[i].transfer_to[j], &stops[i].transfer_time[j]);
        }
    }

    fclose(pathways_file);

    int l;
    for(int i=0; i< nb_routes; i++){
        fscanf(routes_file, "%d %d %d", &routes[i].id, &routes[i].nb_stops, &routes[i].nb_trips);
        routes[i].stops = new int[routes[i].nb_stops];
        routes[i].trips = new int[routes[i].nb_trips * routes[i].nb_stops];

        for(int j=0; j<routes[i].nb_stops; j++){
            fscanf(routes_file, "%d ", &tmp);
            routes[i].stops[j] = tmp;

            for(l=0; stops[tmp].routes[l] != -1 ; l++);
            stops[tmp].routes[l] = routes[i].id;
            stops[tmp].route_pos[l] = j;
        }

        for(int j=0; j<routes[i].nb_trips; j++){
            for(int k=0; k<routes[i].nb_stops; k++){
                fscanf(routes_file, "%d ", &tmp);
                routes[i].trips[k + j*routes[i].nb_stops] = tmp;
            }
        }
    }
    fclose(routes_file);

/* ----------------------------------------------------------------------------------------------------
   Algorithm implementation : RAPTOR
*/


    /* DEFINING VARIABLES  */

    int ps_id = 0;        //source stop
    int pt_id = 14;       //target stop
    unsigned int to = 0;  //departure time

    list <unsigned int> t[total_nb_stops];  //array of multilabel lists
                                            //t[i]: list of earliest arrival times at stop i

    unsigned int t_min[total_nb_stops];     //array for earliest known arrival times at each stop
                                            //used for local pruning

    bool marked[total_nb_stops];  //array for marking stops

    int Q[nb_routes];                //array of routes where Q[r] = p means that
                                    //route r should be processed starting with stop p
                                    
    for(int i=0; i<total_nb_stops; i++)
      marked[i] = false;

    for(int i=0; i<nb_routes; i++)
      Q[i] = -1;

    int k = 0;                              //Round number

    /* INITIALIZING VARIABLES */

    for(int i=0; i<total_nb_stops; i++){    //initialize earliest arrival times with infinity
        t_min[i] = INF;
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
