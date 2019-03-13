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
    return routes[r_id].nb_stops;
  }

  int Network::stop_nb_routes(int s_id){
    return stops[s_id].nb_routes;
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
