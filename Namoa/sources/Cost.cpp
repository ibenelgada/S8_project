
#include "Cost.h"


Cost::Cost(){
  distance = INF;
  height_diff = INF;
}

Cost::Cost(double a, double b){
  distance = a;
  height_diff = b;
}

bool operator<(const Cost& cost1, const Cost& cost2){
  return (cost1.distance <= cost2.distance) && (cost1.height_diff <= cost2.height_diff) && ((cost1.distance != cost2.distance) || (cost1.height_diff != cost2.height_diff));
}

bool operator==(const Cost& cost1, const Cost& cost2){
  return (cost1.distance == cost2.distance) && (cost1.height_diff == cost2.height_diff);
}

bool operator<=(const Cost& cost1, const Cost& cost2){
  return !(cost1 > cost2);
}

bool operator>=(const Cost& cost1, const Cost& cost2){
  return !(cost1 < cost2);
}

bool operator>(const Cost& cost1, const Cost& cost2){
  return (cost1.distance >= cost2.distance) && (cost1.height_diff >= cost2.height_diff) && ((cost1.distance != cost2.distance) || (cost1.height_diff != cost2.height_diff));
}

bool operator!=(const Cost& cost1, const Cost& cost2){
  return !(cost1 == cost2);
}

Cost operator+(const Cost& cost1, const Cost& cost2){
  Cost res;
  res.distance = cost1.distance + cost2.distance;
  res.height_diff = cost1.height_diff + cost2.height_diff;
  return res;
}

std::ostream& operator<<(std::ostream& os, const Cost& cost){
    os << "(" << cost.distance << ", " << cost.height_diff << ")";
    return os;
}
