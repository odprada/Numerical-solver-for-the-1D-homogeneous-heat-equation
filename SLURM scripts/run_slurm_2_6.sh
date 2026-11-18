#!/bin/bash
#
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --time=02:00:00
#SBATCH --job-name=heat_equation
#SBATCH --comment="Run mpi from config"
#SBATCH --output=out_2.txt
#SBATCH --error=error_2.txt

echo "N,p,method,T,steps,time" > results_2.txt

for p in $(seq 2 6); do
    for N in 2000 10000 50000 1000000; do
        #naive O(p) communication
        mpirun -np $p ./a.out $N 0 >> results_2.txt
        
        #odd_even O(1) communication
        mpirun -np $p ./a.out $N 1 >> results_2.txt
    done
done