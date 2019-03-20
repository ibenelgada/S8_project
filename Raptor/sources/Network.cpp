#include "Network.h"

using namespace std;

Network::Network (const string& froutes, const string& fpaths ){
  ifstream sroutes, spaths;

  sroutes.open(froutes.c_str());
  spaths.open(fpaths.c_str());

  init(sroutes, spaths);

  sroutes.close();
  spaths.close();
}

Network::Network(istream& sroutes, istream& spaths){
  init(sroutes, spaths);
}

int Network::nb_routes(){
  return routes.size();
}

int Network::nb_stops(){
  return stops.size();
}

int Network::route_nb_stops(int r_id){
  return routes[r_id].stops.size();
}

int Network::stop_nb_routes(int s_id){
  return stops[s_id].routes.size();
}

std::vector<int>& Network::stop_routes(int s_id){
  return stops[s_id].routes;
}

int Network::route_pos(int r, int p) const{
  /*
  Gives the position of a stop s in a route r,
  used to check if one stop comes before another stop
  */
  for(int i=0; i<stops[p].nb_routes; i++){
      if(stops[p].routes[i] == r)
          return stops[p].route_pos[i];
  }

  return -1;
}

float Network::getCost(int r, int trip, int p1, int p2) const{
  float price = 0.5;
  return price * abs((float)(this->route_pos(r, p1) - this->route_pos(r, p2)));
}

int Network::et(int r, int pi, unsigned int t_pi) const{
  /*
  Returns the earliest trip that can be taken from the stop pi in the route
  r at the departure time t_pi
  */
  int pi_pos = this->route_pos(r, pi);
  unsigned int t_arr;

  for(int t=0; t<routes[r].nb_trips; t++){
    t_arr = routes[r].trips[t * routes[r].nb_stops + pi_pos];
    if(t_pi <= t_arr)
    return t;
  }
  return -1;
}

void Network::get_trips(int r, int pi, unsigned int t_pi, std::vector<int> &trips) const{

  int t;
  t = this->et(r, pi, t_pi);

  for(int i=t; i< this->routes[r].nb_trips; i++){
    trips.push_back(i);
  }

}

void Network::init(istream& sroutes, istream& spaths){
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
