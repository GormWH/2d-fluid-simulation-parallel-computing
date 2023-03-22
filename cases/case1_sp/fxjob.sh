#!/bin/sh
#PJM -L "rscgrp=lecture"
#PJM -L "node=4"
#PJM --mpi "proc=1"
#PJM -L "elapse=00:01:00"
#PJM -j

mpiexec ../../Debug/abmac2d case1.txt
