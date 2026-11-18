#!/bin/bash
#
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=01:00:00
#SBATCH --job-name=heat_equation
#SBATCH --comment="Run mpi from config"
#SBATCH --output=out.txt
#SBATCH --error=error.txt

echo "N,p,method,time" > results.txt

for N in 2000 10000 50000; do
    mpirun -np 1 ./a.out $N 1 >> results.txt
done