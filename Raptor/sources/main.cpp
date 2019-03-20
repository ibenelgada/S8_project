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

#define INF UINT_MAX

using namespace std;



int main(){

    Network myNetwork("data/routes.txt", "data/pathways.txt");

    vector <Route> &routes = myNetwork.routes;
    vector <Stop> &stops = myNetwork.stops;


    int nb_routes = myNetwork.nb_routes();
    int total_nb_stops = myNetwork.nb_stops();
/* ----------------------------------------------------------------------------------------------------
   Algorithm implementation : RAPTOR
*/


    /* DEFINING VARIABLES  */

    int ps_id = 0;        //source stop
    int pt_id = 14;       //target stop
    unsigned int to = 0;  //departure time

    Bags B(total_nb_stops);   //vector of nb_stops maps B(p) where B(p)
                              //is a map of bags B(p,k), round k is the key
                              // usage: B(p,k)

    vector <Bag> bags_star(total_nb_stops);

    vector <bool> marked(total_nb_stops,false);  //vector for marking stops

//
//    marked could be also an empty array where stops to process are added
//

    vector <int> Q(nb_routes, -1);   //vector of routes where Q[r] = p means that
    //Q.reserve(total_nb_stops);     //route r should be processed starting with stop p

    int k = 0;                              //Round number

    /* Some temporary variables */
    int p, r, pp, pi, p_pos, pi_pos;
    string tmp_s;
    bool marked_any;
    Bag Br;
    Bag::const_iterator c_it;
    Bag::iterator it;
    Label l_tmp;
    vector<int> trips;

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
        Q.assign(nb_routes,-1);
        cout << "Clearing Q" << endl;

        //Loop through marked stops only
        for(p=0; p<myNetwork.nb_stops(); ++p){
            if(!marked[p])
                continue;

            //Loop through routes serving the current marked stop p
            for(int j=0; j<myNetwork.stop_nb_routes(p); j++){
                r = stops[p].routes[j];   //route

                if(Q[r] != -1){           //if route r is in Q
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
        for(int i=0; i < nb_routes; ++i){
            if(Q[i]!= -1)
              cout << "(" << i << "," << Q[i] << ")" << endl;
        }
        cout << endl;


        //Processing each route in Q
        for(r=0; r<nb_routes; ++r){
            if(Q[r] == -1)
                continue;

            cout << "----------------------------------------------------------------------------------------------------processing route " << r << " starting with stop " << Q[r] << endl;

            p = Q[r];
            p_pos = myNetwork.route_pos(r,p);

            Br.clear();

            //foreach stop pi in r beginning with stop p
            for(int j=p_pos; j<routes[r].nb_stops; ++j){

                pi = routes[r].stops[j];
                cout << "---------------------------------------------------------------------------Stop " << pi << endl;
                pi_pos = j;

                cout << "Br is:" << endl << Br << endl;
                cout << "B* is:" << endl << bags_star[pi] << endl;

                cout << "-------------------------traversing Br labels" << endl;
                for(it=Br.begin(); it!=Br.end(); ++it){



                  //--------------------------------------------------------
                //  cin >> s;
                  Label& l_ref = *it;

                  l_ref.time = routes[r].trips[l_ref.trip * routes[r].nb_stops + pi_pos];
                  l_ref.price = l_ref.prev_label->price + myNetwork.getCost(r, l_ref.trip, l_ref.hop_stop, pi);

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
                  cout << *c_it << endl;

                  trips.clear();
                  myNetwork.get_trips(r, pi, c_it->time, trips);

                  cout << "trips is of size " << trips.size() << endl;

                  for(unsigned int i=0; i<trips.size(); ++i){
                    cout << "trip " << trips[i] << ":" << endl;
                    l_tmp.fill(c_it, r, trips[i], pi);
                    if(Br.push_nondom(l_tmp))
                      cout << "Label added to Br" << endl;
                    else
                      cout << "Label not added to Br" << endl;

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
