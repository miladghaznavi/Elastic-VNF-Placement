# Elastic-VNF-Placement
This project is the source code of experimentation of "Elastic Virtual Network Function Placement (EVNFP)" research paper published in 4th IEEE conference on Cloud Networking (CloudNet) 2015. The paper introduces Elastic Virtual Network Function Placement (EVNFP) problem and present a model for minimizing operational costs in providing VNF services. In this model, the _elasticity overhead_ and the _trade-off between bandwidth and host resource consumption_ are considered together, while the previous works ignored this perspective of the problem. A solution called Simple Lazy Facility Location (SLFL) is proposed to optimize the placement of VNF instances in response to on-demand workload. SLFL solution is compared to First-Fit and Random placements.

This project contains four folders:
- _Datacenter_: Source code to generate datacenter topology. Fattree and VL2 
- _DemandGen_: Tools to generate demands.
- _GraphTools_: Tools to generate graphs for the results after running the simulation!
- _Source_: The implementation of SLFL, and simulation.
