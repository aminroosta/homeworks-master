#include<string>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<algorithm>
#include<cmath>
using namespace std;

#define N_PLUS_K_MAX 17
#define C_MAX 100
#define min(a,b) ((a) < (b) ? (a) : (b))

#define INF (100*1000*1000)
#define rep(i,f,t) for(int i = (f), end = (t); i < t; ++i)


float dp[ 1 << N_PLUS_K_MAX ][ N_PLUS_K_MAX ][ C_MAX + 1 ];
int xs[ N_PLUS_K_MAX ];
int ys[ N_PLUS_K_MAX ];
int demands[ N_PLUS_K_MAX ];
float costs[ N_PLUS_K_MAX ][ N_PLUS_K_MAX ]; /* distance from node i to node j */

int main() {
  int k, /* number of trucks */
      n, /* number of customers */
      c; /* capacity of each truck */

  string lables; /* text labels such as NODE_COORD_SECTION which are useless */
  cin >> k >> n >> c;

  //k = k - 1; // FIX: for ****ing testcase issue

  if(n+k > N_PLUS_K_MAX || c > C_MAX) {
    cerr << "Sorry its a large problem, we can't help you!" << endl;
    return 0;
  }

  /* read the positions */
  int inx;
  cin >> lables;
  rep(i,0,n) 
    cin >> inx >> xs[inx-1] >> ys[inx-1];

  /* read the demands */
  cin >> lables;
  rep(i,0,n)
    cin >> inx >> demands[inx-1];

  /* read depot location */
  //cin >> lables; //cin >> xs[0] >> ys[0];

  /* copy k-1 extra depots */
  rep(i,n,n+k) {
    xs[i] = xs[0];
    ys[i] = ys[0];
  }

  /* caclulate costs */
  rep(i,0,n+k)
    rep(j, i+1, n+k) {
      costs[i][j] = costs[j][i] = sqrt( (xs[i]-xs[j])*(xs[i]-xs[j]) + (ys[i]-ys[j])*(ys[i]-ys[j]) );
      if((i == 0 || i >= n) && j >= n)
        costs[i][j] = costs[j][i] = INF; /* depots don't have a route to each other */
    }

  /* run the dp algorithm */
  rep(i,0, 1 << (n+k))
    rep(j,0,n+k)
      rep(r,0,c+1)
        dp[i][j][r] = -1;

  dp[1][0][c] = 0;
  rep(i,1, 1 << (n+k)) /* which nodes have been seen */
    rep(j,0,n+k) /* last node */
      rep(r,0,c+1) { /* remaining capacity */
        if(dp[i][j][r] == -1) continue;
        rep(q,0,n+k) { /* check remaining nodes */
          if(i & (1 << q)) continue;

          int rem = (q == 0 || q >= n) ? c : r - demands[q];
          if(rem < 0) continue;

          int p = i | (1 << q);
          if(dp[p][q][rem] == -1) dp[p][q][rem] = dp[i][j][r] + costs[j][q];
          if(dp[p][q][rem] > dp[i][j][r] + costs[j][q]) dp[p][q][rem] = dp[i][j][r] + costs[j][q];
        }
      }

  double ans = INF;
  int everyone = (1 << (n+k))-1;
  rep(r,0,c+1)
    if(dp[everyone][n][r] > 0) {
      ans = min(ans, dp[everyone][n][r]);
    }

  cout << "===> result: " << ans << endl;

  return 0;
}
