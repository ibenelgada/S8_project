#include <iostream>
#include <stdio.h>
#include <list>
#include <iterator>
#include <tuple>
#include <vector>
#include <map>
#include <climits>
#include <fstream>
#include <cmath>
#include "Stop.h"
#include "Route.h"
#include "Network.h"
#include "Position.h"
#include "Label.h"
#include "Bag.h"
#include "Bags.h"
#include "RouteInfo.h"

#define INF UINT_MAX

using namespace std;

int main(){

    Network myNetwork("data/routes.txt", "data/pathways.txt");

    long long id_source;
    long long id_target;


    map<long long, Route> &routes = myNetwork.routes;
    map<long long, Stop> &stops = myNetwork.stops;

    long long nb_routes = myNetwork.nb_routes();
    long long total_nb_stops = myNetwork.nb_stops();

    cout << "nb_routes " << nb_routes << endl;
    cout << "total_nb_stops " << total_nb_stops << endl;

    // map<long long, Route>::iterator it_test;
    // for(it_test=routes.begin(); it_test!=routes.end(); ++it_test){
    //   cout << "---------------------- id_route: " << it_test->first << endl;
    //   cout << "nb_stops: " << it_test->second.nb_stops << endl;
    //   cout << "nb_trips: " << it_test->second.nb_trips << endl;
    //   cout << "stops are:" << endl;
    //   for(vector<long long>::iterator i=it_test->second.stops.begin(); i!=it_test->second.stops.end(); ++i){
    //     cout << *i << " ";
    //   }
    //   cout << endl;
    //   cout << "trips are:" << endl;
    //   for(long long t=0; t<it_test->second.nb_trips; ++t)
    //     for(long long p=0; p<it_test->second.nb_stops; ++p){
    //       cout << it_test->second.trips[t][p] << " ";
    //     }
    // }

/* ----------------------------------------------------------------------------------------------------
   Algorithm implementation : RAPTOR
*/


    /* DEFINING VARIABLES  */

    long long ps_id = 0;        //source stop   //7440; //pessac/alouette
    long long pt_id = 14; //3763; // 2945;  //merignac aeroport      //target stop
    unsigned long long to = 0;  //departure time

    Bags B(total_nb_stops);   //vector of nb_stops maps B(p) where B(p)
                              //is a map of bags B(p,k), round k is the key
                              // usage: B(p,k)

    vector <Bag> bags_star(total_nb_stops);

    map<long long, bool> marked;  //map for marking stops

//
//    marked could be also an empty array where stops to process are added
//

    map<long long, long long> Q;   //map of routes where Q[r] = p means that
    //Q.reserve(total_nb_stops);     //route r should be processed starting with stop p

    int k = 0;                              //Round number

    /* Some temporary variables */
    long long i,p, r, pp, pi, p_pos, pi_pos;
    string tmp_s;
    bool marked_any;
    Bag Br;
    Bag::const_iterator c_it;
    Bag::iterator it;
    Label l_tmp;
    vector<long long> trips;
    map<long long, long long>::iterator sr_it;
    map<long long, Stop>::iterator s_it;
    map<long long, Route>::iterator r_it;

    /* INITIALIZING VARIABLES */

    Label l(to, 0.0);

    B(ps_id,0).push_nondom(l);
    bags_star[ps_id].push_nondom(l);

    //mark the source stop ps
    marked[ps_id] = true;

    while(true){

        //increment round number
        k++;

        cout << endl << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "              ROUND " << k << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl << endl;

        marked_any = false;

        //Clear Q
      //  Q.assign(nb_routes,-1);
        Q.clear();
        cout << "Clearing Q" << endl;

        //Loop through marked stops only
          for(s_it=stops.begin(); s_it!=stops.end(); ++s_it){

            p = s_it->first;  //stop id
            if(marked.find(p)==marked.end() || marked[p]==false)
              continue;

            //Loop through routes serving the current marked stop p
            for(sr_it=stops[p].routes.begin(); sr_it != stops[p].routes.end(); ++sr_it){
                r = sr_it->first;        //route_id

                if(Q.find(r)!=Q.end()){           //if route r is in Q
                    pp = Q[r];
                    //if pp comes before p in route r
                    if(myNetwork.route_pos(r,p) >= myNetwork.route_pos(r,pp))
                        continue;
                }
                Q[r] = p;       //add (r,p) to Q
            }
            marked[p] = false;  //unmark p
        }  //fill Q

        //DISPLAY routes
        cout << "Routes (r,p) in Q are : " << endl;

        for(r_it=routes.begin(); r_it!=routes.end(); ++r_it){
          i = r_it->first;
          if(Q.find(i)!= Q.end())
            cout << "(" << i << "," << Q[i] << ")" << endl;
        }
        cout << endl;


        //Processing each route in Q

         for(r_it=routes.begin(); r_it!=routes.end(); ++r_it){
           r = r_it->first;
           if(Q.find(r) == Q.end())
              continue;

            cout << "----------------------------------------------------------------------------------------------------processing route " << r << " starting with stop " << Q[r] << endl;

            p = Q[r];
            p_pos = myNetwork.route_pos(r,p);

            Br.clear();

            //foreach stop pi in r beginning with stop p
            for(long long j=p_pos; j<routes[r].nb_stops; ++j){

                pi = routes[r].stops[j];
                cout << "---------------------------------------------------------------------------Stop " << pi << endl;
                pi_pos = j;

                cout << "Br is:" << endl << Br << endl;
                cout << "B* is:" << endl << bags_star[pi] << endl;

                cout << "-------------------------traversing Br labels" << endl;
                for(it=Br.begin(); it!=Br.end(); ++it){

                  //--------------------------------------------------------
                  Label& l_ref = **it;

                  l_ref.g.time = routes[r].trips[l_ref.info->trip][pi_pos];
                  l_ref.g.price = l_ref.prev_label->g.price + myNetwork.getCost(r, l_ref.info->trip, l_ref.info->hop_stop, pi);

                  cout << l_ref << endl;

                  if( l_ref <= bags_star[pt_id] && bags_star[pi].push_nondom(l_ref) ){

                      B(pi,k).push_nondom(l_ref);
                      marked[pi] = true;
                      marked_any = true;
                      cout << "adding Label to bag and marking " << pi << endl;

                  }else{
                      cout << "Label not added" << endl;
                  }
                } //Br Label loop

                cout << "-------------------------end of Br" << endl;
                cout << "B* of stop " << pi << " now is:" << endl;
                cout << bags_star[pi] << endl;

                cout << "-------------------------traversing B*" << endl;

                for(c_it=bags_star[pi].cbegin(); c_it!=bags_star[pi].cend(); ++c_it){
                  cout << **c_it << endl;

                  trips.clear();
                  myNetwork.get_trips(r, pi, (*c_it)->g.time, trips);

                  cout << "trips is of size " << trips.size() << endl;

                  for(unsigned long long i=0; i<trips.size(); ++i){
                    cout << "trip " << trips[i] << ":" << endl;

                    l_tmp.fill(*c_it, r, trips[i], pi);

                    if(Br.push_nondom(l_tmp))
                      cout << "Label added to Br" << endl;
                    else
                      cout << "Label not added to Br" << endl;
                    cout << Br << endl;
                  } // trips

                } //add B*(pi) to Br

                cout << "-------------------------end of B*" << endl;
                cout << "Br now is:" << endl;
                cout << Br << endl;


              } //route stops loop

            } //route loop


        //If no stops are marked, end
        if(!marked_any)
            break;

    } //rounds

    cout << endl << "------------------------------------" << endl;
    cout << "         End of algorithm"<< endl;
    cout << "------------------------------------" << endl << endl;


    cout << bags_star[pt_id] << endl;

    return 0;
}
