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

class Stop{

public:

    int id;
    //position pos;
    int nb_routes;
    int nb_transfers;
    std::vector <int> transfer_to;
    std::vector <int> transfer_time;
    std::vector <int> routes;
    std::vector <int> route_pos;

public:

    void set(int p_id, int p_nb_routes, int p_nb_transfers);

    void add_transfert(int to, unsigned int t);

    void add_route();



};

#endif
