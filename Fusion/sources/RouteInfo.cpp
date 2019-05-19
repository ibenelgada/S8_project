
#include "RouteInfo.h"


RouteInfo::RouteInfo():route(-1),trip(-1){}
// RouteInfo::RouteInfo(long long r, long long t, long long s){
//   route = r;
//   trip = t;
//   hop_stop = s;
// }

bool operator==(const RouteInfo& i1, const RouteInfo& i2){
  return (i1.route == i2.route) && (i1.trip == i2.trip);
}

std::ostream& operator<<(std::ostream& os, const RouteInfo& i){

  os << "RouteInfo trip: " << i.trip << " route: " << i.route;
  return os;
}
