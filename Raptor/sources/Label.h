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
  unsigned int time;
  float price;

  // array of class type critirium ?

  int hop_stop;
  int route;
  int trip;
  std::list<Label>::iterator prev_label;

public:

  Label();
  friend bool operator<(const Label&, const Label&);
  friend bool operator==(const Label&, const Label&);
  friend bool operator<=(const Label&, const Label&);
  friend bool operator>=(const Label&, const Label&);
  friend bool operator>(const Label&, const Label&);
  friend bool operator!=(const Label&, const Label&);

  friend std::ostream& operator<<(std::ostream&, const Label&);
};

#endif
