#ifndef ROUTE_H
#define ROUTE_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <climits>
#include <fstream>

class Route{

public:

    long long id;
    long long nb_stops;
    long long nb_trips;
    std::vector <long long> stops;
    std::vector < std::vector<long long> > trips; //trips[t][s]
    std::vector <int> costs;

public:
    void set(long long p_id, long long p_nb_stops, long long p_nb_trips);

};

#endif
