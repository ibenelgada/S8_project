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

    int id;
    int nb_stops;
    int nb_trips;
    std::vector <int> stops;
    std::vector <int> trips;
    std::vector <int> costs;

public:
    void set(int p_id, int p_nb_stops, int p_nb_trips);

};

#endif
