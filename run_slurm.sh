#!/bin/bash
#
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=4
#SBATCH --time=01:00:00
#SBATCH --job-name=heat_equation
#SBATCH --comment="Run mpi from config"
#SBATCH --output=out.txt
#SBATCH --error=error.txt

echo "N,p,method,time" > results.txt

for p in $(seq 1 12); do
    for N in 2000 10000 50000; do
        #naive O(p) communication
        mpirun -np $p ./a.out $N 0 >> results.txt
        
        #odd_even O(1) communication
        mpirun -np $p ./a.out $N 1 >> results.txt
    done
done