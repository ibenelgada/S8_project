
#include "Label.h"

bool operator<(const Label& lb1, const Label& lb2){
  return (lb1.time <= lb2.time) && (lb1.price <= lb2.price) && ((lb1.time != lb2.time) || (lb1.price != lb2.price));
}

bool operator==(const Label& lb1, const Label& lb2){
  return (lb1.time == lb2.time) && (lb1.price == lb2.price) && (lb1.journey.trip == lb2.journey.trip);
}

bool operator<=(const Label& lb1, const Label& lb2){
  return !(lb1 > lb2);
}

bool operator>=(const Label& lb1, const Label& lb2){
  return !(lb1 < lb2);
}

bool operator>(const Label& lb1, const Label& lb2){
  return (lb1.time >= lb2.time) && (lb1.price >= lb2.price) && ((lb1.time != lb2.time) || (lb1.price != lb2.price));
}

bool operator!=(const Label& lb1, const Label& lb2){
  return !(lb1 == lb2);
}

std::ostream& operator<<(std::ostream& os, const Label& lb){
    os << "Label time: " << lb.time << ", price: " << lb.price << ", trip: " << lb.journey.trip << ", prev_stop: " << lb.journey.prev_stop << ", route: " << lb.journey.route;
    return os;
}
