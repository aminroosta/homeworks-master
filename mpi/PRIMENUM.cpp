#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "big_int.hpp"




#define MAX_SIZE 100000000
#define WHEEL 30
#define PI_NUM 3

unsigned long long getNextNumForMathAlgorithm (){
    static unsigned long long num = 1;

    if(num % 6 == 5 )
        num += 2 ;
    else
        num += 4 ;

    return num ;
}

unsigned long long getNextNumForWheelAlgorithm(int *circle){
    static unsigned long long w_num = WHEEL + 1;

    int rem ;

    while (true){
        rem = w_num % WHEEL ;

        if(rem == 0)
            rem = WHEEL ;

        if(circle[rem] == 0){
            w_num += 2 ;
            break ;
        }
        else
            w_num += 2 ;
    }

    return w_num - 2  ;
}

int main(int argc, char **argv){
    MPI_Status status;

    int err , procs;//Error and Number our processes
    int myId ;//ID for each process

    //Initialize MPI
    err = MPI_Init (&argc , &argv);
    if (err  != MPI_SUCCESS){
        printf("\nError initializing MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, err);
    }

    //Number of processes and their ID
    MPI_Comm_size (MPI_COMM_WORLD , &procs);
    MPI_Comm_rank (MPI_COMM_WORLD , &myId);


    unsigned long long n = atoi(argv[1]) ;
    int myAlgorithm = atoi(argv[2]);

    unsigned long long num;
    unsigned long long i = 1;
    unsigned long long j ;
    unsigned long long req = 0;
    unsigned long long *prime ;
    unsigned long long index = 2 ;

    int p[PI_NUM] = {2 , 3 , 5} ;

    prime = malloc(MAX_SIZE * sizeof(unsigned long long)) ;

    prime[0] = 2 ;
    prime[1] = 3 ;

    double starttime, endtime;

    //Algorithm with 6n +- 1
    if(myAlgorithm == 0){

        if(myId == 0){
            starttime = MPI_Wtime();
            n = n - 2 ;
            while(n > 0){
                MPI_Recv (&num , 1, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD , &status);

                if(num > 0){
                    prime[index++] = num ;
                    n--;
                }

                num = getNextNumForMathAlgorithm() ;

                MPI_Send (&num, 1, MPI_UNSIGNED_LONG_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD) ;
            }

            for(j = 1 ; j < procs ; j++){
                MPI_Recv (&num , 1, MPI_UNSIGNED_LONG_LONG, j, 0, MPI_COMM_WORLD , &status);
                if(num > 0)
                    prime[index++] = num ;
                num = 0;
                MPI_Send (&num, 1, MPI_UNSIGNED_LONG_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD) ;

            }

        }

        else{
            MPI_Send (&req, 1, MPI_LONG, 0 , 0, MPI_COMM_WORLD) ;
            MPI_Recv (&num , 1, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD , &status);

            while(num > 0){
                for(j = 2 ; j <= sqrt(num) ; j++)
                    if(num % j == 0){
                        num = 0;
                        break;
                    }
                 MPI_Send (&num, 1, MPI_UNSIGNED_LONG_LONG, 0 , 0, MPI_COMM_WORLD) ;
                 MPI_Recv (&num , 1, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD , &status);
            }


        }

    }


    //Algorithm with wheel sieve
    else{

        if(myId == 0){
            starttime = MPI_Wtime();
            int k ;
            int *cir = malloc((WHEEL + 1) * sizeof(int)) ;

            for(k = 1 ; k <= WHEEL ; k++)
                cir[k] = 0 ;

            for(k = 0 ; k < PI_NUM ; k++)
                for(j = 2 ; p[k] * j <= WHEEL ;j++)
                    cir[ p[k] * j ] = 1 ;

            for(k = 5 ; k <= WHEEL ; k++ )
                if(cir[k] == 0)
                    prime[index++] = k ;

            cir[2] = 1 ;
            cir[3] = 1 ;
            cir[5] = 1 ;
            n = n - 10 ;

            while(n > 0){
                MPI_Recv (&num , 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD , &status);

                if(num > 0){
                    prime[index++] = num ;
                    n--;
                }

                num = getNextNumForWheelAlgorithm(cir) ;
                //printf("num = %ld\n" , num);
                MPI_Send (&num, 1, MPI_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD) ;
            }

            for(j = 1 ; j < procs ; j++){
                MPI_Recv (&num , 1, MPI_LONG, j, 0, MPI_COMM_WORLD , &status);
                if(num > 0)
                    prime[index++] = num ;
                num = 0;
                MPI_Send (&num, 1, MPI_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD) ;

            }

        }

        else{
            MPI_Send (&req, 1, MPI_LONG, 0 , 0, MPI_COMM_WORLD) ;
            MPI_Recv (&num , 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD , &status);

            while(num > 0){
                for(j = 2 ; j <= sqrt(num) ; j++)
                    if(num % j == 0){
                        num = 0;
                        break;
                    }
                 MPI_Send (&num, 1, MPI_LONG, 0 , 0, MPI_COMM_WORLD) ;
                 MPI_Recv (&num , 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD , &status);
            }


        }


    }


    if(myId == 0){

        endtime   = MPI_Wtime();
        int count = 0 ;
        /*for(j = 0 ; j < index ; j++){
            printf("%llu\t" , prime[j]);
            count++;
            if(count == 10){
                printf("\n");
                count = 0;
            }
        }*/
        printf("\n");

        printf("Execution Time : %f\n",endtime-starttime);
    }

    free(prime);
    MPI_Finalize();
    return 0;
}
