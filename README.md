# Elastic-VNF-Placement
This repository releases the source-code and the simulator developed as part of "Elastic Virtual Network Function Placement (EVNFP)"[1] published in 4th IEEE conference on Cloud Networking (CloudNet) 2015. The paper introduces _Elastic Virtual Network Function Placement_ (EVNFP) problem and presents a mathematical model to optimize the operational costs in providing VNF services.
In this model, the _elasticity overhead_ and the _trade-off between bandwidth and host resource consumption_ are considered together, while prior work ignored this perspective of the problem. We propose a solution called _Simple Lazy Facility Location_ (SLFL) to optimize the placement of VNF instances in response to on-demand workload. In our evaluation, we comapre SLFL with _First-Fit_ and _Random_ placements.

This repository contains four folders:
- _Datacenter_: Source code to generate datacenter topologies: Fattree and VL2.
- _DemandGen_: Tools to generate service demands.
- _GraphTools_: Tools to plot the results of the simulation!
- _Source_: SLFL's implementation and the simulator.

## _Datacenter_ folder
This folder contains the source code to generate data-center topologies. The source code uses Lemon graph library (https://github.com/bekaus/lemon-1.2.1). The following topologies are implemented.
- Fat-tree [2]
- VL2 [3]

`main.h` implements a command tool to generate Fat-tree topology. For instance, to generate a fat-tree topology is (it is assumed that after the compilation the execution file is `DCTopo`):
```bash
./DCTopo -k 10 -l 1000 -h 8 -o dc.txt
```
This command generates a text file called `dc.txt` in the working directory which contains `10`-Ary fat-tree. Each link has the capacity of `1000 Mbps` and each host has `8` core cpu.

## _DemandGen_ folder
The source codes for generating and visualizing VNF demands are located in this folder. The folder contains two files:
-`request.py`
-`draw.py`

### `request.py`
For generating the VNF demands, you can use following command:
```bash
python request.py -c 1000 -k 10 -a 1 -d 3600 -s 10 -o demands.txt 
```
This command generates 10000 demand (`-d 1000`) for a 10-Ary fat-tree topology (`-k 10`). The arrival rate of demands is 1 request per second (`-a 1`) drawn from poisson distribution. The duration of each demand is in average 3600 seconds (`-d 3600`) drawn from exponential distribution. The random seed is set to 10 (`-s 10`), and the request file is saved in demands.txt (`-o demands.txt`).

### `draw.py`
To visualize the workload of the generated demands, following command can be used:
```bash
python draw.py -i demands.txt -l log.txt -d demands.html
```
This command reads the demands.txt file (`-i demands.txt`) and generates a log file (`-l log.txt`) and a html file (`demands.html`). The log file is a csv file containing the workload over time. The html file is a visual representation of the demands.

## _GraphTools_ folder
This folder contains three files:
- `draw.py`
- `draw_in_bunch.py`
- `graph.py`

### `draw.py`
This file is used to generate the charts in EPS format and html format. The file uses matplotlib library and NVD3 library. For the available option look at the source code.

### `draw_in_bunch.py`
This file generate all reported charts in the paper. For the available option look at the source code.

### `graph.py`
This file is the script used to generate the charts reported in the paper. For the available option look at the source code.

## _Source_ folder
This folder contains the main source implementing the SLFL algorithm and First-Fit and Random placement. After compiling the program, by following command you can run the experiment (It is assumed that the execution file is called `VNFPlacement`):
```bash
VNFPlacement -c path/to/config.ini -a slfl -v
```
The command run the experiment by reading the configuration file config.ini (`-c path/to/config.ini`) and using SLFL algorithm (`-a slfl`) in the verbose mode (`-v`). For the implemented algorithms, use the following parameters:
- `-a slfl` for the SLFL algorithm
- `-a firstfit` for the First-Fit algorithm
- `-a random` for the Random algorithm

# References
[1] M. Ghaznavi, A. Khan, N. Shahriar, K. Alsubhi, R. Ahmed, R. Boutaba. Elastic Virtual Network Function Placement. IEEE International Conference on Cloud Networking (CloudNet). Niagara Falls (Canada), October 5-7, 2015.

[2] Al-Fares, Mohammad, Alexander Loukissas, and Amin Vahdat. "A scalable, commodity data center network architecture." ACM SIGCOMM Computer Communication Review 38.4 (2008): 63-74.

[3] Greenberg, Albert, et al. "VL2: a scalable and flexible data center network." ACM SIGCOMM computer communication review. Vol. 39. No. 4. ACM, 2009.
