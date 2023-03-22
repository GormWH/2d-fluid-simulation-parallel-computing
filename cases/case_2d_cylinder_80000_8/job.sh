#!/bin/sh
#PJM -g gt66
#PJM -L "rscgrp=lecture"
#PJM -L "node=1"
#PJM --mpi "proc=8"
#PJM -L "elapse=00:15:00"
#PJM -j

mpiexec.hydra -n ${PJM_MPI_PROC} ../../../Release/abmac2d ../case.txt
