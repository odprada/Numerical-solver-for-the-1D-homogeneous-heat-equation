#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

static double u_newtime(double u, double u_previous, double u_next, double k_const, double tau_const, double h_const){
    return u + (k_const * tau_const / (h_const * h_const)) * (u_next - 2 * u + u_previous);
}

int main(int argc, char *argv[]) {
    int myrank, size;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    double k, tau, h;
    k = 1;
    h = 0.02;
    tau = 0.0002;
    long N = 1/h + 1;
    long array_size = N/size;
    long rem = N % size;
    long local_size = 2 + array_size + (myrank < rem ? 1 : 0);
    double u_local[local_size];
    double u_local_new[local_size];

    
    for (long i=1; i<local_size-1; i++){
        u_local[i] = 1;
    }

    //boundary condition prevails at (0,0), making the value at the endpoints zero at any time
   
    if (myrank == 0){
        u_local[1] = 0;
    }

    if (myrank == size-1){
        u_local[local_size-2] = 0;
    }

    MPI_Finalize();
    return 0;
}