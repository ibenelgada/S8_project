
#include "Position.h"


Position::Position():lat(0), lon(0), alt(0){}


std::ostream& operator<<(std::ostream& os, Position& pos){
  os << "(" << pos.lat << ", " << pos.lon << ")";
  return os;
}
