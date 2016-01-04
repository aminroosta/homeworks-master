#include <functional>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <numeric>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <stdio.h>
#include <cstdio>
#include <time.h>
#include <vector>
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
typedef pair<int, int> pii;
void init(int, char**, int&, int&); /* prototype declaration */

int procs, id;
template<typename T>
void bcast(T* p, const int n){
    MPI_Bcast(p, n*sizeof(T), MPI_BYTE, 0, MPI_COMM_WORLD);
}
void bcast(pii& p){
    bcast(&p,1); return;
    static int buf[2] = {  };
    buf[0] = p.first; buf[1] = p.second;

    printf("%d>\n",id);
    MPI_Bcast(buf, sizeof(int)*2, MPI_INT, 0, MPI_COMM_WORLD);
    printf("%d<\n",id);
    p.first = buf[0]; p.second = buf[1];
}

void send(pii& p,int worker) {
    static int buf[2];
    buf[0] = p.first; buf[1] = p.second;
    MPI_Send(buf,2, MPI_INT, worker, 0, MPI_COMM_WORLD);
}

pii receive(int from =  MPI_ANY_SOURCE){
    static MPI_Status status;
    static int buf[2] = {  };

    MPI_Recv(buf, 2, MPI_INT, from, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    //int mpi_source;
    //mpi_source = status. MPI_SOURCE;
    return pii(buf[0],buf[1]);
}
const int mx = 1000 + 10;
const int inf = INT_MAX;

int adj[mx][mx];
int adj_raw[mx][3];
int dist[mx];
priority_queue<pii, vector<pii>, greater<pii> > pq;


int main(int argc, char **argv) {
    init(argc, argv, procs,id);
    char input[mx] = "input";
    int n, m;
    int v,u, w;
    if(!id) {
        cout << "Enter input file: "; cin >> input;

        stdin = freopen(input, "r", stdin);

         cin >> n >> m;
        rep(i, 0, m) {
            cin >> v >> u >> w;
            adj_raw[i][0] = v;
            adj_raw[i][1] = u;
            adj_raw[i][2] = w;
        }
    }
    bcast(&n,1); bcast(&m,1);
    bcast((int*)adj_raw,3*m);
    rep(i,0,m){
        v = adj_raw[i][0];
        u = adj_raw[i][1];
        w = adj_raw[i][2];
        adj[v][u] = adj[u][v] = w;
    }
    repe(i, 1, n)
        dist[i] = inf/4;
    dist[1] = 0;

    if(id) { /* workers */
        for(;/*ever*/;){
            pii tmp;
            bcast(tmp);
            //printf("worker:%d %d %d\n",id, tmp.first,tmp.second);
            if(tmp.second == -1) break;
            v = tmp.second, w = tmp.first;
            pii ret(inf/4,-1);
            repse(u,id,n,procs-1) {
                //for(int u = id; u <=n; u += procs-1)
                if(adj[v][u] && (w + adj[v][u] <= dist[u])) {
                     dist[u] = w + adj[v][u];
                     if(dist[u] < ret.first)
                         ret = pii(dist[u],u);
                }
            }
            send(ret, 0);
            //printf("ret:%d %d %d\n",id, ret.first,ret.second);
        }

    }
    else { /* master */
        pq.push(make_pair(0, 1));
        pii tmp;
        //rep(i,0,n){
        while (!pq.empty()){
            tmp = pq.top(); pq.pop();
            v = tmp.second; w = tmp.first; dist[v] = w;
            //printf("master: %d %d\n", tmp.second, tmp.first);
            bcast(tmp);
            rep(worker,1, procs) {
                tmp = receive();
                if(tmp.second == -1) continue;
                //printf("got: %d %d\n", tmp.second, tmp.first);
                pq.push(tmp);
            }
        }
        tmp.second = -1;
        bcast(tmp);
        printf("done\n");
    }

    if(!id)
        repe(i,1,n)
            cout << "node " << char(i + 'A' - 1) << " " << dist[i] << endl;

    MPI_Finalize();
    return 0;
}

void init(int argc, char **argv, int& procs, int& id) {

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
}
