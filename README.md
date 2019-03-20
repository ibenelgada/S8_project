# S8 project
Optimal paths computing algorithm

Compile program:

		make
  
Run program:

		bin/raptor

### Problem statement

We study the problem of computing all optimal paths from a source point to a target point using multiple ways of transport (public transport, bicycle, car, walking..), and taking into consideration many criteria other than time of arrival (price, number of transfers, physical effort..). 
Two types of transport can be distinguished:
Constrained transport: which involves public transport; bus, train, tramway, subway..
unconstrained transport:  that isn’t constrained by time, like walking, driving..
The algorithm developed should be able to find a path while combining constrained and unconstrained ways of transport. This requires the combination of two best path algorithms, one for each type of transport. 
We should note that since we consider more than one criterion to find paths, there will be a number of  “best paths” that cannot be compared to each other and can all be considered optimal paths, these are called the Pareto-set. The objective of the algorithm is to find all Pareto-optimal paths in multi-modal transportation

# ! Description
The algorithm developed is based on two existing algorithms:
RAPTOR (Round-Based Public Transit Routing algorithm), for constrained transport. 
NAMOA* (Multi-objective A*) for unconstrained transport.
