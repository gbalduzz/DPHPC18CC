import networkx as nx
import sys
import random
import io, json

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
nr_graphs = 1
if (len(sys.argv) > 1 and sys.argv[1].isdigit()):
	nr_graphs = int(sys.argv[1]) 
else: 
	print("Choose default and create 1 graph")

for i in range(nr_graphs): 
	#generate graph size
	nr_edges = random.randint(1e7,1e8)
	nr_nodes = random.randint(1e7,1e7)	
	
	G = nx.gnm_random_graph(nr_nodes,nr_edges)
	nr_connected_components = nx.number_connected_components(G)

	print("Graph ", i, " has : ", nr_connected_components)
	graph_name = "graph_" + str(i)
	nx.write_adjlist(G,graph_name +".adjlist")

	"""
	Choose first node of each connected component as representative and write it into the info.txt file. 
	Line x with number z means that node x belongs to the graph with representative z. 	
	"""
	connected_components = nx.connected_components(G) #### NECESSARY !!!
	representatives = [None] * nr_nodes
	for connected_component in connected_components: 
		for l in connected_component: 
			representatives[l] = list(connected_component)[0]

	with open(graph_name + "_info.txt", 'w') as f: 
		for x in representatives: 
			f.write(str(x) + "\n")

	f.close()




		

	

