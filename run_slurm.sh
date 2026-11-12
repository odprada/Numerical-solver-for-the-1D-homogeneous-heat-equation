#!/bin/bash
#
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --job-name=heat_equation
#SBATCH --comment="Run mpi from config"
#SBATCH --output=out.txt
#SBATCH --error=error.txt

> results.txt

for N in 2000 10000 50000; do
      echo $N | mpirun -np 1 ./a.out | grep RESULT >> results.txt
dones