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
#include <map>

#include "Route.h"
#include "Stop.h"

class Network{
public:
  std::map<long long, Route> routes;
  std::map<long long, Stop> stops;

public:

  Network (const std::string& froutes, const std::string& fpaths );
  Network(std::istream& sroutes, std::istream& spaths);

  long long nb_routes();

  long long nb_stops();

  long long route_nb_stops(long long r_id);

  long long  stop_nb_routes(long long s_id);

  std::map<long long, long long>& stop_routes(long long s_id);

  long long route_pos(long long r, long long p) const;

  double getCost(long long r, long long trip, long long p1, long long p2) const;

  long long et(long long r, long long pi, double t) const;

  void get_trips(long long r, long long pi, double t_pi, std::vector<long long> &trips) const;

private:

  void init(std::istream& sroutes, std::istream& spaths);

};

#endif
