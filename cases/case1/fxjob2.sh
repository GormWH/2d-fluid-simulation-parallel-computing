#!/bin/sh
#PJM -g gt66
#PJM -L "rscgrp=lecture"
#PJM -L "node=1"
#PJM --mpi "proc=4"
#PJM -L "elapse=0:15:00"
#PJM -j

#------- Program execution -------#
mpiexec.hydra -n ${PJM_MPI_PROC} ../../../Debug/abmac2d ../case2.txt
