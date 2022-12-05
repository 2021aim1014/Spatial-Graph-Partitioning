# Spatial-Graph-Partitioning

Instruction to run program-

1. Type the below command to compile the program on linux platform
	$ g++ -std=c++17 2021AIM1014.cpp

2. After successful compilation, run the below command
	$ ./a.out

3. After program gets executed, a folder gets created - Data. It 	    contains the data blocks which includes the partition data.

4. To execute the program again, delete the data folder, and    		continue from step 1.

Makes sure to have 2 files:
1. nodes.txt - node_id x_cordinate y_cordinate
2. edges.txt - source destination edge_length
Above file names can be changed appropriately in the code, at line number 13 and 14

block size = 100
K = 1000
Above size can be changed appropriately in the code, at line number 10 and 11.
