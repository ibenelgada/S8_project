#include "Bag.h"


using namespace std;

Bag::Bag(){

}

Bag::Bag(list<Label*>& b){
  bag = b;
}

size_t Bag::size() const{
  return bag.size();
}

bool Bag::empty() const{
  return bag.empty();
}


void Bag::pop_back(){
  bag.pop_back();
}

void Bag::pop_front(){
  bag.pop_front();
}


void Bag::clear(){
  bag.clear();
}

Bag::iterator Bag::begin(){
  return bag.begin();
}

Bag::iterator Bag::end(){
  return bag.end();
}

Bag::const_iterator Bag::cbegin() const{
  return bag.cbegin();
}

Bag::const_iterator Bag::cend() const{
  return bag.cend();
}

bool operator<(const Label& l, const Bag& b){
  Bag::const_iterator it;
  for(it = b.cbegin(); it != b.cend(); ++it){
    if( **it <= l )
      return false;
  }
  return true;
}

bool operator>(const Label& l, const Bag& b){
  Bag::const_iterator it;
  for(it = b.cbegin(); it != b.cend(); ++it){
    if( **it >= l )
      return false;
  }
  return true;
}

bool operator<=(const Label& l, const Bag& b){
  Bag::const_iterator it;
  for(it = b.cbegin(); it != b.cend(); ++it){
    if( **it < l )
      return false;
  }
  return true;
}

bool operator>=(const Label& l, const Bag& b){
  Bag::const_iterator it;
  for(it = b.cbegin(); it != b.cend(); ++it){
    if( **it > l )
      return false;
  }
  return true;
}


bool operator<(const Bag& b, const Label& l){
  return l > b;
}

bool operator>(const Bag& b, const Label& l){
  return l < b;
}

bool operator<=(const Bag& b, const Label& l){
  return l >= b;
}

bool operator>=(const Bag& b, const Label& l){
  return l <= b;
}

bool Bag::push_nondom(Label* l){
  Bag::iterator it = bag.begin();

  while(it != bag.end()){
    if( **it < *l || **it == *l )
      return false;
    if( **it > *l)
      it = bag.erase(it);
    else
      ++it;
  }
  Label* label = new Label;
  *label = *l;
  bag.push_back(label);
  return true;
}

bool Bag::push_nondom(Label* l, Label*& new_l){
  Bag::iterator it = bag.begin();

  while(it != bag.end()){
    if( **it < *l || **it == *l )
      return false;
    if( **it > *l)
      it = bag.erase(it);
    else
      ++it;
  }
  Label* label = new Label;
  *label = *l;
  bag.push_back(label);
  new_l = label;
  return true;
}

std::ostream& operator<<(std::ostream& os, const Bag& bag){
    if(bag.bag.size() == 0){
      os << "Empty Bag" << std::endl;
      return os;
    }

    os << "Bag of size " << bag.bag.size() << ":" << std::endl;
    int i = 1;
    for(std::list<Label*>::const_iterator it = bag.bag.begin(); it != bag.bag.end(); ++it){
      os << "  " << i << ") " << **it << std::endl;
      ++i;
    }
    return os;
}
