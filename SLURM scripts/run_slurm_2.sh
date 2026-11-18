#!/bin/bash
#
#SBATCH --nodes=3
#SBATCH --ntasks-per-node=4
#SBATCH --time=02:00:00
#SBATCH --job-name=heat_equation_j2
#SBATCH --comment="Run mpi from config"
#SBATCH --output=out_2.txt
#SBATCH --error=error_2.txt

echo "N,p,method,T,steps,time" > results_2.txt

for p in $(seq 9 12); do
    for m in 0 1; do
        mpirun -np $p ./a.out 2000 $m 0.1 >> results_2.txt
        mpirun -np $p ./a.out 10000 $m 0.002 >> results_2.txt
        mpirun -np $p ./a.out 50000 $m 0.00002 >> results_2.txt
        mpirun -np $p ./a.out 250000 $m 0.0000004 >> results_2.txt
        mpirun -np $p ./a.out 1000000 $m 0.00000001 >> results_2.txt
    done
done