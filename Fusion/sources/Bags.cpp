
#include "Bags.h"



Bags::Bags(int nb_stops){
  bags.resize(nb_stops);
}


Bag& Bags::operator()(int p, int k){
  if(bags[p].find(k) == bags[p].end()){
    bags[p][k] = Bag();
  }
  return bags[p][k];
}

bool Bags::empty(int p, int k) const{
  return bags[p].find(k) == bags[p].end();
}

std::ostream& operator<<(std::ostream& os, const Bags& bags){
  return os;
}
