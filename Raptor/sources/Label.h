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

  RouteInfo* info;

  Label* prev_label;

public:

  Label();
  Label(long long time, double price);
  Label& operator=(Label &l);
  void fill(Label* c_it, long long r, long long t, long long s);
  //void Label::fill(Label* c_it, RouteInfo* info_ptr);

  friend bool operator<(const Label&, const Label&);
  friend bool operator==(const Label&, const Label&);
  friend bool operator<=(const Label&, const Label&);
  friend bool operator>=(const Label&, const Label&);
  friend bool operator>(const Label&, const Label&);
  friend bool operator!=(const Label&, const Label&);

  friend std::ostream& operator<<(std::ostream&, const Label&);
};

#endif
