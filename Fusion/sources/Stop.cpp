#include "Stop.h"


void Stop::set(long long p_id, long long p_nb_routes, long long p_nb_transfers){
  id = p_id;
  nb_routes = p_nb_routes;
  nb_transfers = p_nb_transfers;
}

void Stop::add_transfert(long long to, unsigned long long t){
  transfer_to.push_back(to);
  transfer_time.push_back(t);
}

void Stop::add_route(){

}
