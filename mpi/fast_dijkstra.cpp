#include <functional>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <string>
#include <cstdio>
#include <vector>
#include <queue>
#include <deque>
#include <cmath>
#include <list>
#include <set>
#include <map>
#define rep(i,m,n) for(int i=(m),_end=(n);i < _end;++i)
#define repe(i,m,n) for(int i=(m), _end =(n);i <= _end;++i)
using namespace std;
typedef pair<int, int> pii;

const int mx = 1000 + 10;
const int inf = INT_MAX;

vector<pii> adj[mx];
int dist[mx];
priority_queue<pii, vector<pii>, greater<pii> > pq;


int main() {
    cout << "Enter input file: ";
    string input; cin >> input;
    stdin = freopen(input.c_str(), "r", stdin);

    int n, m; cin >> n >> m;

    repe(i, 1, n)
        dist[i] = inf,
        adj[i].clear();
    int v,u, w;
    rep(i, 0, m)
        cin >> v >> u >> w,
            adj[v].push_back(make_pair(w, u)),
            adj[u].push_back(make_pair(w, v));

    dist[1] = 0;
    pq.push(make_pair(0, 1));
    pii tmp;
    while (!pq.empty()){
        tmp = pq.top(); pq.pop();
        v = tmp.second; w = tmp.first;
        if (w > dist[v]) continue;
        rep(j, 0, adj[v].size())
        {
            u = adj[v][j].second;
            int w2 = adj[v][j].first;

            if (w + w2 < dist[u])
                dist[u] = w + w2,
                    pq.push(make_pair(w + w2, u));

        }

    }
    repe(i,1,n)
        cout << "node " << char(i + 'A' - 1) << " " << dist[i] << endl;

    return 0;

}
