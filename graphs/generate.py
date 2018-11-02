import networkx as nx
import sys
import random

"""
GENERATES RANDOM GRAPHS

use as: 
****
generate.py nr_of_graphs
nr_of_graphs has to be an integer

returns: 
****
adjacency lists of generated graphs.
File name: test_graphNr_NrOfConnectedComponents.adj
graphNr is the number of the generated graph
NrOfConnectedComponents is the number of connected components in the respective graph. 

"""

if (len(sys.argv) == 1): 
	print("Requires the number of graphs to generate as an argument. ")
	print("Try something like ")
	print("python3 generate.py 10")
	quit()
if (sys.argv[1].isdigit() == False): 
	print("Nr of graphs should be an int")
	print("Try something like ")
	print("python3 generate.py 10")
	quit()

nr_graphs = int(sys.argv[1])
print(nr_graphs)

for i in range(nr_graphs): 
	#generate graph size
	nr_edges = random.randint(100,1000)
	nr_nodes = random.randint(100,200)	
	
	G = nx.gnm_random_graph(nr_nodes,nr_edges)
	nr_connected_components = nx.number_connected_components(G)

	print("Graph ", i, " has : ", nr_connected_components)
	nx.write_adjlist(G,"graph_"+ str(i) + "_" + str(nr_connected_components) +".adjlist")
