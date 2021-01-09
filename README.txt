========================
CS377P Assignment 4

by Yi-Shan Lu, 3/6/2018
========================

1. The implementation depends on icc and VTune.
   To set the environment variables, run:
   source set_env_hw4.sh

2. To compile the code, run one of the following:
   make
   make CXXFLGAS=-DSP2018_CS377P_STRUCT_OF_ARRAYS

3. To run the program, run the following:
   ./pagerank <input.dimacs> <pull|push> <threshold>

4. To profile using VTune, run:
   source run_vtune.sh

   When finished, examine the VTune results for the following configurations:
   r000 - rmat22, AoS, pull
   r001 - rmat22, AoS, push
   r002 - rmat22, SoA, pull
   r003 - rmat22, SoA, push
   r004 - rmat22.transposed, AoS, pull
   r005 - rmat22.transposed, AoS, push
   r006 - rmat22.transposed, SoA, pull
   r007 - rmat22.transposed, SoA, push
   r008 - roadUSA, AoS, pull
   r009 - roadUSA, AoS, push
   r010 - roadUSA, SoA, pull
   r011 - roadUSA, SoA, push
