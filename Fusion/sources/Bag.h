#ifndef BAG_H
#define BAG_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <climits>
#include <fstream>

#include "Label.h"

class Bag;


std::ostream& operator<<(std::ostream&, const Bag&);

bool operator<(const Label& l, const Bag& b);
bool operator>(const Label& l, const Bag& b);
bool operator<=(const Label& l, const Bag& b);
bool operator>=(const Label& l, const Bag& b);

bool operator<(const Bag& b, const Label& l);
bool operator>(const Bag& b, const Label& l);
bool operator<=(const Bag& b, const Label& l);
bool operator>=(const Bag& b, const Label& l);



class Bag{

public:
  std::list <Label*> bag;

public:

  Bag();
  Bag(std::list<Label*>&);

  typedef std::list<Label*>::iterator iterator;
  typedef std::list<Label*>::const_iterator const_iterator;

  bool push_nondom(Label* l);
  bool push_nondom(Label* l, Label*& new_l);
  size_t size() const;
  bool empty() const;

  void pop_back();
  void pop_front();


  void clear();

  iterator begin();
  iterator end();
  const_iterator cbegin() const;
  const_iterator cend() const;

  friend std::ostream& operator<<(std::ostream&, const Bag&);

};



#endif
