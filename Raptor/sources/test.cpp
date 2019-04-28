#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <climits>
#include <cstdlib>
#include <map>

#include "Bag.h"
#include "Label.h"

using namespace std;

class ab{

  public:

    int a;

    void set(const int& i){
      a = i;
    }

};

int main()
{

  list<int> a;
  a.push_back(5);
  a.push_back(6);
  a.push_back(7);

  list<int>::iterator it;

  it = a.begin();
  it = a.erase(it);
  it--;
  it++;

  for(it = a.begin(); it != a.end(); it++){
    cout << "jij" << endl;
  }

  cout << a.size() << endl;
  cout << "it == begin: " << (it == a.begin()) << endl;
  cout << "it == end: " << (it == a.end()) << endl;
  cout << *it << endl;

  return 0;
}
