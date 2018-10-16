# Important decisions #
Tools
- C++
- boost (threading)
- cmake
- google test


Architecture
- Xeon (whatever)

Verification
- unit tests
- comparison against paper

Bounds / Comparison
- compare against paper

Use cases
- Wikipedia (https://en.wikipedia.org/wiki/Maximum_flow_problem#Real_world_applications)
- Ask TA for test data!

Key concepts

Obstacles


# Papers #
- https://arxiv.org/pdf/1501.03105.pdf 
	- A parallel min-cut algorithm using iteratively reweighted least squares 
	- Yao Zhu and David F. Gleich, Purdue University
- https://stanford.edu/~rezab/classes/cme323/S16/projects_reports/flatow_penner.pdf
	- A distributed algorithm for global min cut
	- David Flatow and Daniel Penner, Stanford University
- http://spcl.inf.ethz.ch/Publications/.pdf/comm_avoiding_mincut.pdf
	- Communication avoiding parallel minimum cuts and connected components
	- Lukas Gianinazzi, Pavel Kalvoda et al., ETH Zürich
	
# Who does what? #
- graph implementation
	- storage 
	- operations
	
- sequential algorithm implementation

- parallel implementation
	- distribution to multiple nodes 
	- multithreading on single node
	
- testing
	- correctness
	- performance
	
- writing / plotting
