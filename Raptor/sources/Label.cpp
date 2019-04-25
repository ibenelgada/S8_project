
#include "Label.h"

Label::Label():time(0),price(0.0),hop_stop(-1),route(-1),trip(-1),prev_label(nullptr){}
Label::Label(long long time, double price):time(time),price(price),hop_stop(-1),route(-1),trip(-1),prev_label(nullptr){}

void Label::fill(std::list<Label>::const_iterator c_it, long long r, long long t, long long s){
  *this = *c_it;
  prev_label = c_it;
  trip = t;
  route = r;
  hop_stop = s;
}


bool operator<(const Label& lb1, const Label& lb2){
  return (lb1.time <= lb2.time) && (lb1.price <= lb2.price) && ((lb1.time != lb2.time) || (lb1.price != lb2.price));
}

bool operator==(const Label& lb1, const Label& lb2){
  return (lb1.time == lb2.time) && (lb1.price == lb2.price) && (lb1.trip == lb2.trip);
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
    os << "Label time: " << lb.time << ", price: " << lb.price << ", trip: " << lb.trip << ", prev_stop: " << lb.hop_stop << ", route: " << lb.route;
    return os;
}
