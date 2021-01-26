# Demonstration code for reordering using pagerank algo

To know the difference between the proposed reordering and the actual ordering of graph, issue the following commands:

Compile the program as:

``` g++ ./src/pagerank.cpp ./src/graph.cpp ./src/graph.h -o pagerank
```

Run the pagerank program:

``` ./pagerank <input-graph-dimacs-file> <no-of-nodes> <no-of-edges> <pull | push> <threshold>
```

``` <pull | push> ``` 
is used to used to determine the type of computation for pagerank should be pull-type or push-type

``` <threshold>  ``` 
used inside convergence criterion of the pagerank algo


**Note:** Currently reordering is implemented with push type computation of pagerank

## Generating the .dimacs from .el

There is a Python script inside ```/inputs/``` folder ```create_dimacs.py``` 

Command to run this script:

```python create_dimacs.py <input.el> <no_of_nodes> <no_of_edges>

## Issues

1. This one is figured out by observation: The .dimacs files should be such that all the vertex values are contiguous and like 0, 1, 2... and not like 3, 56, 57...
