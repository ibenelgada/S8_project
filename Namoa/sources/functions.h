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
#include <ctime>
#include <sstream>
#include <chrono>


#include "Position.h"
#include "Label.h"
#include "Cost.h"
#include "Arc.h"
#include "Node.h"
#include "Graph.h"

#include "Json.h"


std::string Namoa(Graph& myGraph, std::map<long long, Position>& nodes, long long start_node, long long end_node);

double getDistance(std::map<long long,Position> &nodes, long long nd1, long long nd2);

double getHeightDiff(std::map<long long,Position> &nodes, long long nd1, long long nd2);

Cost getCost(std::map<long long,Position> &nodes, long long nd1, long long nd2);

void filter(std::list<std::pair<Label*,Cost>>& open_all, std::map<long long, std::list<Label*>>& open, Label* lbp);

bool dominated(Cost eval, const std::list<Label*>& label_list);

void rmDominated(std::list<Label*>& labels, Label* lbp);
void rmDominated(std::list<std::pair<Label*,Cost>>& labels, Label* lbp);
bool isNondom(std::list<std::pair<Label*,Cost>>& open, std::list<std::pair<Label*,Cost>>::iterator lb_it);

bool isNondom(std::list<Label*>& open, Label* label);

std::list<std::pair<Label*,Cost>>::iterator getNondomNode(std::list<std::pair<Label*,Cost>>& open);

bool add_nondom(std::list<Label*>& labels, Label* lbp);

bool produce_cycle(long long m, Label* current_label);

void remove( std::vector<long long>& vect, long long id);

std::vector<long long> getNodes(Label* label);

std::string to_json(std::list<Label*>& labels);

void init_graph_complete(Graph& myGraph, std::map<long long,Position> & nodes, std::string graph_file_str, std::string nodes_file_str);

void get_start_end(const std::string& in_file, long long& start_node, long long& end_node);
