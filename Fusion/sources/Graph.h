#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <map>

#include "Node.h"
#include "Arc.h"

class Graph;

std::ostream& operator<<(std::ostream&, const Graph&);

class Graph{

public:

  std::map<long long, std::map<long long,class Arc> > nodes;

public:

  friend std::ostream& operator<<(std::ostream&, const Graph&);
};

#endif
