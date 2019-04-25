#ifndef STOP_H
#define STOP_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <climits>
#include <fstream>
#include <map>

class Stop{

public:

    long long id;

    long long nb_routes;
    long long nb_transfers;
    std::vector <long long> transfer_to;
    std::vector <long long> transfer_time;
    std::map<long long, long long> routes;  // <route_id, position in route>

public:

    void set(long long p_id, long long p_nb_routes, long long p_nb_transfers);

    void add_transfert(long long to, unsigned long long t);

    void add_route();



};

#endif
