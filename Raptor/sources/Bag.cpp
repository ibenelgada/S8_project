
#include "Bag.h"



size_t Bag::size() const{
  return bag.size();
}

bool Bag::empty() const{
  return bag.empty();
}


std::ostream& operator<<(std::ostream& os, const Bag& bag){
    if(bag.bag.size() == 0){
      os << "Empty Bag" << std::endl;
      return os;
    }

    os << "Bag of size " << bag.bag.size() << ":" << std::endl;
    int i = 1;
    for(std::list<Label>::const_iterator it = bag.bag.begin(); it != bag.bag.end(); ++it){
      os << "  " << i << ") " << *it << std::endl;
      ++i;
    }
    return os;
}
