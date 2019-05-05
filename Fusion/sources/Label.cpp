
#include "Label.h"
#include <vector>
#include <string>
#include <sstream>
using namespace std;

Label::Label():prev_label(nullptr){}
// Label::Label(long long n, long long time, double price):prev_label(nullptr),info(-1,-1,-1),g(0.0,0.0,time,price),node(n){
//   info = new RouteInfo(-1,-1,-1);
//   node = n;
//   Cost* g_ptr = new Cost(0.0,0.0,time,price);
//   g = *g_ptr;
// }
//
Label::Label(long long n):prev_label(nullptr),node(n){}

void Label::fill(Label* c_it, long long r, long long t){

  *this = *c_it;
  prev_label = c_it;
//  g.k = c_it->g.k + 1;
  info.trip = t;
  info.route = r;
}

 bool operator<(const Label& lb1, const Label& lb2){
   return (lb1.g < lb2.g);
 }

 bool operator==(const Label& lb1, const Label& lb2){
   return (lb1.g == lb2.g) && (lb1.info.trip == lb2.info.trip);
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
     os << "cost " << lb.g << ", trip: " << lb.info.trip << ", prev_stop: " << (lb.prev_label == NULL ? -1 : lb.prev_label->node) << ", route: " << lb.info.route;
     return os;
 }


 string Label::to_path(){
   Label* l = this;
   long long route = info.route;
   vector<Label*> lbs;

   while(l != nullptr && route == l->info.route){
     lbs.push_back(l);
     l = l->prev_label;
   }


   Label* start = lbs.back();
   Label* end = lbs.front();

   start = (start->prev_label == nullptr) ? start: start->prev_label;

   stringstream ss;

   if( route < 0)
    ss << "from " << start->node << " to " << end->node << " arriving at " << end->g.time << endl;
   else
    ss << "route: " << end->info.route << " trip: " << end->info.trip << " from stop: " << start->node << " to stop " << end->node << " arriving at " << end->g.time << endl;


  if(l != NULL)
    return l->to_path() + ss.str();

  return ss.str();
 }
