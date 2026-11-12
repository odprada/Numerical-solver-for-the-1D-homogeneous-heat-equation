#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#define PI 3.14159265358979323846

static double u_newtime(double u, double u_previous, double u_next, double k_const, double tau_const, double h_const){
    return u + (k_const * tau_const / (h_const * h_const)) * (u_next - 2 * u + u_previous);
}

static double u_exact(double x, double t, double k_const) {
    double sum = 0.0;
    for (int m = 0; m < 100; m++) {
        double n = 2 * m + 1;
        sum += 4.0 / (n * PI) * sin(n * PI * x) * exp(-k_const * n * n * PI * PI * t);
    }
    return sum;
}

int main(int argc, char *argv[]) {
    int myrank, size;
    MPI_Status Status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    double k, tau, h, T;
    k = 1;
    long N = 51;

    if (argc > 1) N = atol(argv[1]);

    if (N == 51){
        T = 0.1;
        h = 0.02;
        tau = 0.0002;
    } else {
        T = 0.0001;
        h = 1.0 / (N - 1);
        tau = 1.0 / (2.0 * N * N);
    }

    long array_size = N/size;
    long rem = N % size;
    long steps = (long)(T/tau + 0.5);
    long local_size = 2 + array_size + (myrank < rem ? 1 : 0);

    //malloc for o(1) pointer swap
    double *u_local     = malloc(local_size * sizeof(double));
    double *u_local_new = malloc(local_size * sizeof(double));
    if (u_local == NULL || u_local_new == NULL) {
        fprintf(stderr, "rank %d: malloc failed\n", myrank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (long i=1; i<local_size-1; i++){
        u_local[i] = 1;
    }

    //Zero boundary conditions
    if (myrank == 0){
        u_local[1] = 0;
        u_local_new[1] = 0;
        u_local[0] = 0;
        u_local_new[0] = 0;
    }

    if (myrank == size-1){
        u_local[local_size-2] = 0;
        u_local_new[local_size-2] = 0;
        u_local[local_size-1] = 0;
        u_local_new[local_size-1] = 0;
    }

    for (long n = 0; n < steps; n++){
        //1. Exchange
        if (myrank % 2 != 0) {
            if (myrank > 0){
                MPI_Send(&u_local[1], 1, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD);
            }
            if (myrank > 0){
                MPI_Recv(&u_local[0], 1, MPI_DOUBLE, myrank-1, 2, MPI_COMM_WORLD, &Status);
            }
            if (myrank < size-1){
                MPI_Recv(&u_local[local_size-1], 1, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &Status);
            }
            if (myrank < size-1){
                MPI_Send(&u_local[local_size-2], 1, MPI_DOUBLE, myrank+1, 2, MPI_COMM_WORLD);
            }
        }

        else {
            if (myrank < size-1){
                MPI_Recv(&u_local[local_size-1], 1, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &Status);
            }
            if (myrank < size-1){
                MPI_Send(&u_local[local_size-2], 1, MPI_DOUBLE, myrank+1, 2, MPI_COMM_WORLD);
            }
            if (myrank > 0){
                MPI_Send(&u_local[1], 1, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD);
            }
            if (myrank > 0){
                MPI_Recv(&u_local[0], 1, MPI_DOUBLE, myrank-1, 2, MPI_COMM_WORLD, &Status);
            }
        }

        //2. Update
        for (long i = 1; i < local_size-1; i++){
            u_local_new[i] = u_newtime(u_local[i], u_local[i-1], u_local[i+1], k, tau, h);
        }
        if (myrank == 0){
            u_local_new[1] = 0;
        }
        if (myrank == size-1){
            u_local_new[local_size-2] = 0;
        }

        //3. Swap
        double *tmp = u_local;
        u_local = u_local_new;
        u_local_new = tmp;
    }

    long start = myrank * array_size + (myrank < rem ? myrank : rem);
    long owned_points = local_size-2;

    //Display values
    for (long g = 0; g < N; g += 5) {
        if (g >= start && g < start + owned_points) {
            long j = g - start + 1;
            double ue = u_exact(g * h, T, k);
            printf("%ld %.6f %.12f %.12f %.3e\n", g, g * h, u_local[j], ue, fabs(u_local[j] - ue));
        }
    }

    free(u_local);
    free(u_local_new);
    MPI_Finalize();
    return 0;
}