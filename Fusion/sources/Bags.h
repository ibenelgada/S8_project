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
  std::map <long long, std::map <int, Bag > > bags;

public:

  Bags();

  Bag& operator()(long long p, int k);

  bool empty(long long p, int k);

  friend std::ostream& operator<<(std::ostream&, const Bag&);

};



#endif
