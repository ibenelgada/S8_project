
#include "Label.h"


Label::Label(){
  g = Cost(0.0,0.0);
  node = 0;
  prev_label = nullptr;
}


Label::Label(long long n){
  g = Cost(0.0,0.0);
  node = n;
  prev_label = nullptr;
}

bool operator<(const Label& lb1, const Label& lb2){
  return (lb1.g < lb2.g);
}

bool operator==(const Label& lb1, const Label& lb2){
  return (lb1.g == lb2.g);
}

bool operator<=(const Label& lb1, const Label& lb2){
  return !(lb1 > lb2);
}

bool operator>=(const Label& lb1, const Label& lb2){
  return !(lb1 < lb2);
}

bool operator>(const Label& lb1, const Label& lb2){
  return (lb1.g > lb2.g);
}

bool operator!=(const Label& lb1, const Label& lb2){
  return !(lb1 == lb2);
}

std::ostream& operator<<(std::ostream& os, const Label& lb){
os << "Label: node " << lb.node << ", g " << lb.g << ", prev_node " << ( lb.prev_label == nullptr ? -1 : lb.prev_label->node);
    return os;
}
