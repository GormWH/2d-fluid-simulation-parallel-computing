#!/bin/sh
#PJM -g gt66
#PJM -L "rscgrp=interactive"
#PJM -L "node=1"
#PJM --mpi "proc=32"
#PJM -L "elapse=00:15:00"
#PJM -j

mpiexec.hydra -n ${PJM_MPI_PROC} ${I_MPI_DEBUG} ../../../Debug/abmac2d ../case.txt
