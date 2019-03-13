#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <climits>
#include <map>
#include "Stop.h"

#define INF UINT_MAX   //2^32 - 1 (biggest unsigned int)

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

class cmp{
public:

  static int te(int a) {
    cout << a+1 << endl;
  }

};

int main()
{

  Stop a;

  cout << 5 << endl;
  return 0;
}
