#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>

#include "Position.h"
#include "Arc.h"

class Node;

std::ostream& operator<<(std::ostream&, const Node&);

class Node{

public:

  long long id;
  Position pos;
  std::vector<Arc> arcs;

public:

  Node();

  friend std::ostream& operator<<(std::ostream&, const Node&);
};

#endif
