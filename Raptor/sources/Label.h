#ifndef LABEL_H
#define LABEL_H

#include <iostream>
#include <list>

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
  unsigned long long time;
  double price;

  // array of class type critirium ?

  long long hop_stop;
  long long route;
  long long trip;
  
  std::list<Label>::const_iterator prev_label;

public:

  Label();
  Label(long long time, double price);

  void fill(std::list<Label>::const_iterator c_it, long long r, long long t, long long s);

  friend bool operator<(const Label&, const Label&);
  friend bool operator==(const Label&, const Label&);
  friend bool operator<=(const Label&, const Label&);
  friend bool operator>=(const Label&, const Label&);
  friend bool operator>(const Label&, const Label&);
  friend bool operator!=(const Label&, const Label&);

  friend std::ostream& operator<<(std::ostream&, const Label&);
};

#endif
