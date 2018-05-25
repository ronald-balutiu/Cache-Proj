# Based off of Cache Lab from CMSC 15400: Intro to Computer Systems at UChicago.

About:
This project was created in C as part of my Computer Systems class to simulate the properties and actions of a working cache. The mock cache takes in a Valgrind trace file as well as cache parameters (s,E,b) and then outputs the number of hits, misses, and evictions that should have occurred inside of the cache. The lab was originally based off of a CMU lab.

To compile, run:
   
   'linux> make'

To test correctness of simulator against given reference simulator, run:
 
   'linux> ./test-csim'
   
   
Files:
------

Makefile  -  Compiles code 
csim.c    -  Cache simulator
csim-ref  -  Reference cache simulator executable
test-csim -  Runs testing on csim.c
traces/   -  Traces files used as inputs to cache simulators
README    -  This file
