#include "Stop.h"


void Stop::set(int p_id, int p_nb_routes, int p_nb_transfers){
  id = p_id;
  nb_routes = p_nb_routes;
  nb_transfers = p_nb_transfers;
}

void Stop::add_transfert(int to, unsigned int t){
  transfer_to.push_back(to);
  transfer_time.push_back(t);
}

void Stop::add_route(){

}
