#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <climits>
#include <fstream>
#include <cmath>

#include "Route.h"
#include "Stop.h"

class Network{
public:
  std::vector <Route> routes;
  std::vector <Stop> stops;

public:

  Network (const std::string& froutes, const std::string& fpaths );
  Network(std::istream& sroutes, std::istream& spaths);

  int nb_routes();

  int nb_stops();

  int route_nb_stops(int r_id);

  int stop_nb_routes(int s_id);

  std::vector<int>& stop_routes(int s_id);

  int route_pos(int r, int p) const;

  float getCost(int r, int trip, int p1, int p2) const;

  int et(int r, int pi, unsigned int t) const;

  void get_trips(int r, int pi, unsigned int t_pi, std::vector<int> &trips) const;

private:

  void init(std::istream& sroutes, std::istream& spaths);

};

#endif
