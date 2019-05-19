
#include "Bags.h"


using namespace std;
Bags::Bags(){

}


Bag& Bags::operator()(long long p, int k){
  return bags[p][k];
}

bool Bags::empty(long long p, int k) {
  return bags[p].find(k) == bags[p].end();
}

std::ostream& operator<<(std::ostream& os, const Bags& bags){
  return os;
}
