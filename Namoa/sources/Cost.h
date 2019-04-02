#ifndef COST_H
#define COST_H

#include <iostream>
#include <limits>

#define INF std::numeric_limits<double>::infinity()

class Cost;

bool operator<(const Cost&, const Cost&);
bool operator==(const Cost&, const Cost&);
bool operator<=(const Cost&, const Cost&);
bool operator>=(const Cost&, const Cost&);
bool operator>(const Cost&, const Cost&);
bool operator!=(const Cost&, const Cost&);
Cost operator+(const Cost&, const Cost&);
std::ostream& operator<<(std::ostream&, const Cost&);

class Cost{
public:

  double distance;
  double height_diff;

public:

  Cost();
  Cost(double, double);

  friend bool operator<(const Cost&, const Cost&);
  friend bool operator==(const Cost&, const Cost&);
  friend bool operator<=(const Cost&, const Cost&);
  friend bool operator>=(const Cost&, const Cost&);
  friend bool operator>(const Cost&, const Cost&);
  friend bool operator!=(const Cost&, const Cost&);
  friend Cost operator+(const Cost&, const Cost&);

  friend std::ostream& operator<<(std::ostream&, const Cost&);
};

#endif
