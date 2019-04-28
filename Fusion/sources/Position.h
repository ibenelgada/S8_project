#ifndef POSITION_H
#define POSITION_H

#include <iostream>

class Position;

std::ostream& operator<<(std::ostream& os, Position& pos);

class Position{
public:
    double lat;
    double lon;
    double alt;

public:
    Position();

    friend std::ostream& operator<<(std::ostream& os, Position& pos);
};


#endif
