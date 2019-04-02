
#include "Label.h"


bool operator<(const Label& lb1, const Label& lb2){
  return ((lb1.g + lb1.h) < (lb2.g + lb2.h));
}

bool operator==(const Label& lb1, const Label& lb2){
  return (lb1.g == lb2.g) && (lb1.h == lb2.h);
}

bool operator<=(const Label& lb1, const Label& lb2){
  return !(lb1 > lb2);
}

bool operator>=(const Label& lb1, const Label& lb2){
  return !(lb1 < lb2);
}

bool operator>(const Label& lb1, const Label& lb2){
  return ((lb1.g + lb1.h) > (lb2.g + lb2.h));
}

bool operator!=(const Label& lb1, const Label& lb2){
  return !(lb1 == lb2);
}

std::ostream& operator<<(std::ostream& os, const Label& lb){
    os << "not defined" << std::endl;
    return os;
}
