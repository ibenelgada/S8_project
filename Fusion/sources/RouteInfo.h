#ifndef ROUTEINFO_H
#define ROUTEINFO_H

#include <iostream>
class RouteInfo;

std::ostream& operator<<(std::ostream& os, const RouteInfo& i);
bool operator==(const RouteInfo& i1, const RouteInfo& i2);

class RouteInfo{
public:
  long long route;
  long long trip;
public:

  RouteInfo();
//  RouteInfo(long long r, long long t, long long s);

  friend bool operator==(const RouteInfo& i1, const RouteInfo& i2);
  friend std::ostream& operator<<(std::ostream& os, const RouteInfo& i);
};

#endif
