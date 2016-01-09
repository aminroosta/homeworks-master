#include <functional>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <numeric>
#include <cstdlib>
#include <climits>
#include <stdio.h>
#include <cstdio>
#include <time.h>
#include <vector>
#include <string>
#include <math.h>
#include <queue>
#include <deque>
#include <cmath>
#include <mpi.h>
#include <list>
#include <set>
#include <map>
#define rep(i,m,n) for(int i=(m),_end=(n);i < _end;++i)
#define repe(i,m,n) for(int i=(m), _end =(n);i <= _end;++i)
#define repse(i,m,n,s) for(int i=(m), _end=(n),_step=(s); i <= _end; i+=_step)
using namespace std;

struct Args {
    int argc;
    char** argv;
    Args(int _argc, char** _argv) : argc(_argc), argv(_argv) {  }

    template<typename RET, int POS>
    RET get() {
        RET ret;
        get(ret, std::string(argv[POS]));
        return ret;
    }

    void get(double& ret, string str) { ret = strtod(str.c_str(),NULL); }
    void get(float& ret, string str) { ret = strtof(str.c_str(),NULL); }
    void get(int& ret, string str) { ret = atoi(str.c_str()); }
};



void app(int id, int procs,Args args) {
    if(id == 0){
        int m = args.get<int,1>(),
            n = args.get<int,2>(),
            p = args.get<int,3>();
        double a = args.get<double,4>(),
               b = args.get<double,5>();

        cout << id << ' ' << procs << ' ' << m << ' ' << n << ' ' << p << ' ' << a << ' ' << b << endl;
    }
}

int main(int argc, char **argv) {
    int procs = 0, id = 0;

    int err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS){
        printf("\nError initializing MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, err);
        exit(0);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    app(id,procs, Args(argc,argv));

    MPI_Finalize();
    return 0;
}
