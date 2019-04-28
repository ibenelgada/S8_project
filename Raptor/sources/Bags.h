#ifndef BAGS_H
#define BAGS_H

#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <map>
#include <climits>
#include <fstream>

#include "Label.h"
#include "Bag.h"

class Bags;

std::ostream& operator<<(std::ostream&, const Bags&);

class Bags{

private:
  std::vector < std::map <int, Bag > > bags;

public:

  Bags(int nb_stops);

  Bag& operator()(int p, int k);

  bool empty(int p, int k) const;

  friend std::ostream& operator<<(std::ostream&, const Bag&);

};



#endif
