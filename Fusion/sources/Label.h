#ifndef LABEL_H
#define LABEL_H

#include <iostream>
#include <list>
#include "RouteInfo.h"
#include "Cost.h"

class Label;

bool operator<(const Label&, const Label&);
bool operator==(const Label&, const Label&);
bool operator<=(const Label&, const Label&);
bool operator>=(const Label&, const Label&);
bool operator>(const Label&, const Label&);
bool operator!=(const Label&, const Label&);
std::ostream& operator<<(std::ostream&, const Label&);

class Label{
public:

  Cost g;

  long long node;

  RouteInfo info;

  Label* prev_label;

public:

  Label();
//  Label(long long n, long long time, double price);
  Label(long long n);
  void fill(Label* c_it, long long r, long long t);
  std::string to_path();

  friend bool operator<(const Label&, const Label&);
  friend bool operator==(const Label&, const Label&);
  friend bool operator<=(const Label&, const Label&);
  friend bool operator>=(const Label&, const Label&);
  friend bool operator>(const Label&, const Label&);
  friend bool operator!=(const Label&, const Label&);

  friend std::ostream& operator<<(std::ostream&, const Label&);
};

#endif
