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

class Bag{

public:
  std::list <Label> bag;



public:
  typedef std::list<Label>::iterator iterator;

  size_t size() const;
  bool empty() const;


  iterator insert (iterator position, const Label& label);
  iterator erase (iterator position);
  void remove (const Label& label);
  void clear();

  iterator begin();
  iterator end();

    friend std::ostream& operator<<(std::ostream&, const Bag&);

};



#endif
