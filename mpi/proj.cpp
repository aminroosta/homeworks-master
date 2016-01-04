#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <math.h>
#define max_digits_allowed 1000
#include "big_int.hpp"

typedef big_int<uint64> bint; // big integer with internals of uint64 numbers

/* net the next big integer to check for beeing prime */
bint next_num() {
    static bint next = 1;
    static uint64 to_add = 4;

    next += to_add;
    to_add = (to_add == 4) ? 2 : 4;
    return next;
}

/* send a big integer */
void send(const bint& A, int worker){
    MPI_Send(&A.arr[0],A.arr.size(), MPI_UNSIGNED_LONG_LONG, worker, 0, MPI_COMM_WORLD);
}

/* receive a big integer */
bint receive(int& mpi_source, int from =  MPI_ANY_SOURCE){
    static const uint64 max_elements = max_digits_allowed/sizeof(uint64) + 1;
    static uint64 buf[max_elements] = {  };
    static MPI_Status status;

    MPI_Recv(buf, max_elements, MPI_UNSIGNED_LONG_LONG, from, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    mpi_source = status. MPI_SOURCE;

    bint ret = 0;
    int count = status.count/sizeof(uint64);
    if(count){
        ret.arr.pop_back();
        for(int i = 0; i < count; ++i)
            ret.arr.push_back(buf[i]);
    }
    return ret;
}
/* overload if the caller doesn't care about the sender */
bint receive(){ int dummy; return receive(dummy); }

void init(int, char**, int&, int&, int&); /* prototype declaration */


int main(int argc, char **argv){
    const bint zero = 0;

    int procs, id, n;
    init(argc, argv, procs,id,n);

    if(!id) { /* master */
        int counter = 0;
        n = n < (procs-1) ? (procs-1) : n;
        cout << "Generationg max(procs, n) = " << n << " prime numbers ..." << endl;

        for(int worker = 1; worker < procs; ++worker)
            send(next_num(), worker);

        while(counter < n) {
            int who; /* ? */
            bint bi = receive(who); /* get the result */

            if(bi != zero) {
                counter++;
                (counter%1 == 0) && cerr << counter << " => " << bi << endl; /* print to console */
                (counter%1000 == 0) && cout << counter << " => " << bi << endl; /* print to file */
            }

            if(n >= procs-1)
                send(next_num(), who); /* send another number to the same worker */
        }

        /* send termination signal */
        for(int worker = 1; worker < procs; ++worker) {
            int who; /* ? */
            bint bi = receive(who);
            if(bi != zero)
                cout << ++counter << " => " << bi << endl;
            send(zero, who);
        }
    }
    else /* slave */
        for(;/*ever*/;) {
            bint bi = receive();
            if(bi == zero)
                break;
            if(!is_prime(bi))
                bi = zero;
            send(bi, 0);
        }
    //cout << "done " << id << endl;

    MPI_Finalize();
    return 0;
}

void init(int argc, char **argv, int& procs, int& id, int& n) {

    int err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS){
        printf("\nError initializing MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, err);
        exit(0);
    }

    procs = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    id = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // Check for number of arguments
    if(id == 0 && argc-1 != 1)
        printf("ARGUMENT ERROR: pass exactly one argument for N\n");
    if (argc-1 != 1) {
        MPI_Finalize();
        exit(0);
    }
    n = atoi(argv[1]);
}
