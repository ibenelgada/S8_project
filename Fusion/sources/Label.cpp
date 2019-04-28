
#include "Label.h"

using namespace std;

Label::Label():prev_label(nullptr),node(0){
  info = new RouteInfo(-1,-1,-1);
  Cost* g_ptr = new Cost();
  g = *g_ptr;
}
Label::Label(long long n, long long time, double price):prev_label(nullptr){
  info = new RouteInfo(-1,-1,-1);
  node = n;
  Cost* g_ptr = new Cost(0.0,0.0,time,price);
  g = *g_ptr;
}

Label::Label(long long n):prev_label(nullptr){
  info = new RouteInfo(-1,-1,-1);
  node = n;
  Cost* g_ptr = new Cost();
  g = *g_ptr;
}

void Label::fill(Label* c_it, long long r, long long t, long long s){

  *this = *c_it;
  prev_label = c_it;

  info->route = r;
  info->trip = t;
  info->hop_stop = s;
}


 Label& Label::operator=(Label &l){

   RouteInfo* i = new RouteInfo();

   this->g = l.g;
   this->prev_label = l.prev_label;
   this->node = l.node;

   *i = *l.info;
   this->info = i;
   return *this;
 }

 bool operator<(const Label& lb1, const Label& lb2){
   return (lb1.g < lb2.g);
 }

 bool operator==(const Label& lb1, const Label& lb2){
   return (lb1.g == lb2.g) && (lb1.info->trip == lb2.info->trip);
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
     os << "cost " << lb.g << ", trip: " << lb.info->trip << ", prev_stop: " << lb.info->hop_stop << ", route: " << lb.info->route;
     return os;
 }
