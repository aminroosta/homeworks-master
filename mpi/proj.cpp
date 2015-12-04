#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <math.h>

typedef unsigned long long int ull;

void print(ull* arr,int count){
    int i;
    for(i = 0; i < count; ++i){
        printf("%5llu,",arr[i]);
    }
    printf("\n");
}


ull next_num(){
    static ull nxt = 1; // 6*0 + 1
    if(nxt%6 == 5) // 6*k - 1
        nxt += 2;  // 6*k + 1
    else           // 6*k + 1
        nxt += 4;
    return nxt;
}

int main(int argc, char **argv){


    // initialize MPI_Init
    int err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS){
        printf("\nError initializing MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, err);
    } // end if

    int procs = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &procs);	// Get No. of processors

    int id = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);			// Get processor id

    // Check for number of arguments
    if(id == 0 && argc-1 != 1)
        printf("ARGUMENT ERROR: pass exactly one argument for N\n");
    if (argc-1 != 1) {MPI_Finalize(); return 0;}
    int n = atoi(argv[1]);

    // master
    if(id == 0){
        ull *primes = (ull*)malloc(1000*1000*sizeof(ull));
        primes[0] = 2; primes[1] = 3;
        int prime_inx = 2;
        int worker = 0; /* worker id */
        ull k;
        MPI_Status status; // MPI_SOURCE, TAG, ERROR
        // initially add every worker a number to check for beeing a prime
        for(worker = 1; worker < procs; ++worker) {
            k = next_num();
            MPI_Send(&k,1, MPI_UNSIGNED_LONG_LONG, worker, 0, MPI_COMM_WORLD);
        }
        // dynamically give numbers to each worker
        while(n > 0){
            k = next_num() ;
            MPI_Recv(primes + prime_inx, 1, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(primes[prime_inx] != 0)
                ++prime_inx;
            MPI_Send(&k,1, MPI_UNSIGNED_LONG_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            n-- ;
        }
        // terminate workers
        for(worker = 1; worker < procs; ++worker){
            MPI_Recv(primes + prime_inx, 1, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(primes[prime_inx] != 0)
                ++prime_inx;
            k = 0;
            MPI_Send(&k,1, MPI_UNSIGNED_LONG_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        }
        print(primes,prime_inx);
        free(primes);
    }
    // slaves
    else {
        MPI_Status status; // MPI_SOURCE, TAG, ERROR
        ull k;
        for(;/*ever*/;){
            MPI_Recv(&k, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(k == 0) break; // master signaled to terminate

            ull i = 1;
            /*for(i = 1; (6*i-1) * (6*i-1) <= k; ++i)
              if(k % (6*i-1) == 0 || k%(6*i+1) == 0) {
              k = 0; // k is not a prime number
              break;
              }*/

            for(i = 2 ; i*i < k ; i++)
                if(k % i == 0){
                    k = 0 ;
                    break;
                }

            MPI_Send(&k,1, MPI_UNSIGNED_LONG_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
} // end main
