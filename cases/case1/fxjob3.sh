#!/bin/sh
#PJM -g gt66
#PJM -L "rscgrp=lecture"
#PJM -L "node=1"
#PJM --mpi "proc=4"
#PJM -L "elapse=00:01:00"
#PJM -j

mpiexec.hydra -n ${PJM_MPI_PROC} ./../../Debug/abmac2d case1.txt
