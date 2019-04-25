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

long long Network::nb_routes(){
  return routes.size();
}

long long Network::nb_stops(){
  return stops.size();
}

long long Network::route_nb_stops(long long r_id){
  return routes[r_id].stops.size();
}

long long Network::stop_nb_routes(long long s_id){
  return stops[s_id].routes.size();
}

std::map<long long,long long>& Network::stop_routes(long long s_id){
  return stops[s_id].routes;
}

long long Network::route_pos(long long r, long long p) const{
  /*
  Gives the position of a stop p in a route r,
  used to check if one stop comes before another stop
  */
  const Stop& s = stops.at(p);
  if(s.routes.find(r) == s.routes.cend())
    return -1;  //if stop p isn't a part pf route r

  return s.routes.at(r);
}

double Network::getCost(long long r, long long trip, long long p1, long long p2) const{
  double price = 0.5;
  return price * abs((double)(this->route_pos(r, p1) - this->route_pos(r, p2)));
}

long long Network::et(long long r, long long pi, unsigned long long t_pi) const{
  /*
  Returns the earliest trip that can be taken from the stop pi in the route
  r at the departure time t_pi
  */
  long long pi_pos = this->route_pos(r, pi);
  unsigned long long t_arr;

  const Route& route = routes.at(r);

  for(long long t=0; t<route.nb_trips; ++t){
  //  t_arr = route.trips[t * route.nb_stops + pi_pos];
    t_arr = route.trips[t][pi_pos];
    if(t_pi <= t_arr)
      return t;
  }
  return -1;
}

void Network::get_trips(long long r, long long pi, unsigned long long t_pi, std::vector<long long> &trips) const{

  long long t;
  t = this->et(r, pi, t_pi);

  const Route& route = routes.at(r);

  for(long long i=t; i< route.nb_trips; i++){
    trips.push_back(i);
  }

}

// void Network::init(istream& sroutes, istream& spaths){
//   long long nb_routes, total_nb_stops;
//
//   long long s_id, nb_stop_routes, nb_transfers;
//
//   unsigned long long transfer_to, transfer_time;
//
//   long long r_id, nb_stops, nb_trips;
//
//   long long stop, time;
//
//   sroutes >> nb_routes;
//   //sroutes >> total_nb_stops;
//
//   routes.resize(nb_routes);
//   total_nb_stops = 0;
//
//   for(long long i=0; i<nb_routes; i++){
//     sroutes >> r_id;
//     sroutes >> nb_stops;
//     sroutes >> nb_trips;
//
//     total_nb_stops = total_nb_stops + nb_stops;
//
//     stops.resize(total_nb_stops);
//     routes[i].set(r_id, nb_stops, nb_trips);
//
//     for(long long j=0; j<nb_stops; j++){
//       sroutes >> stop;
//       routes[i].stops.push_back(stop);
//       stops[stop].routes.push_back(routes[i].id);
//       stops[stop].route_pos.push_back(j);
//     }
//
//     for(long long j=0; j<nb_trips; j++){
//       for(long long k=0; k<nb_stops; k++){
//         sroutes >> time;
//         routes[i].trips.push_back(time);
//       }
//     }
//
//   }
//   for(long long i=0; i<total_nb_stops; i++){
//     spaths >> s_id;
//     spaths >> nb_stop_routes;
//     spaths >> nb_transfers;
//
//     stops[i].set(s_id, nb_stop_routes, nb_transfers);
//
//     for(long long j=0; j<nb_transfers; j++){
//       spaths >> transfer_to;
//       spaths >> transfer_time;
//
//       stops[i].add_transfert(transfer_to, transfer_time);
//     }
//
//   }
//
// }


void Network::init(istream& sroutes, istream& spaths){

  long long nb_routes;
  long long id_route, nb_stops, nb_trips;
  long long id_stop;

  sroutes >> nb_routes;
  int n(0);
  for(long long i=0; i<nb_routes; ++i){
    sroutes >> id_route >> nb_stops >> nb_trips;
    routes[id_route].set(id_route, nb_stops, nb_trips);

    cout << nb_trips << endl;

    routes[id_route].trips.resize(nb_trips);

    for(long long j=0; j<nb_stops; ++j){
      sroutes >> id_stop;
      ++n;
      stops[id_stop].id = id_stop;
      cout << "size before " << stops[id_stop].routes.size() << endl;
      stops[id_stop].routes[id_route] = j;
      cout << "add route " << id_route << " to stop " << id_stop << " pos is " << j << endl;
      cout << "size after " << stops[id_stop].routes.size() << endl;
      routes[id_route].stops.push_back(id_stop);
    }

    for(long long t=0; t<nb_trips; ++t){
      routes[id_route].trips[t].resize(nb_stops);
      for(long long s=0; s<nb_stops; ++s){
        sroutes >> routes[id_route].trips[t][s];
      }
    }
  }

  cout << "STOPS : " << n << endl;
  std::map<long long, Stop>::iterator it;
  for(it=stops.begin(); it!=stops.end(); ++it){
    Stop& s = it->second;
    s.nb_routes = s.routes.size();

    cout << "stop " << it->first << " has nb_routes " << it->second.nb_routes << endl;
  }

}
