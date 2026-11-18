#!/bin/bash
#
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=4
#SBATCH --time=02:00:00
#SBATCH --job-name=heat_equation_j1
#SBATCH --comment="Run mpi from config"
#SBATCH --output=out_1.txt
#SBATCH --error=error_1.txt

echo "N,p,method,T,steps,time" > results_1.txt

mpirun -np 1 ./a.out 2000 1 0.1 >> results_1.txt
mpirun -np 1 ./a.out 10000 1 0.002 >> results_1.txt
mpirun -np 1 ./a.out 50000 1 0.00002 >> results_1.txt
mpirun -np 1 ./a.out 250000 1 0.0000004 >> results_1.txt
mpirun -np 1 ./a.out 1000000 1 0.00000001 >> results_1.txt

for p in $(seq 2 8); do
    for m in 0 1; do
        mpirun -np $p ./a.out 2000 $m 0.1 >> results_1.txt
        mpirun -np $p ./a.out 10000 $m 0.002 >> results_1.txt
        mpirun -np $p ./a.out 50000 $m 0.00002 >> results_1.txt
        mpirun -np $p ./a.out 250000 $m 0.0000004 >> results_1.txt
        mpirun -np $p ./a.out 1000000 $m 0.00000001 >> results_1.txt
    done
done