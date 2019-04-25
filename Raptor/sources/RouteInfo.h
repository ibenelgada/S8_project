#ifndef ROUTEINFO_H
#define ROUTEINFO_H

#include <iostream>
class RouteInfo;

std::ostream& operator<<(std::ostream& os, const RouteInfo& i);

class RouteInfo{
public:
  long long hop_stop;
  long long route;
  long long trip;
public:

  RouteInfo();
  RouteInfo(long long r, long long t, long long s);

  friend std::ostream& operator<<(std::ostream& os, const RouteInfo& i);
};

#endif
