#include<string>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<algorithm>
#include<cmath>
using namespace std;

#define N_MAX 1000
#define min(a,b) ((a) < (b) ? (a) : (b))

#define INF (100*1000*1000)
#define rep(i,f,t) for(int i = (f), end = (t); i < t; ++i)


int xs[ N_MAX ];
int ys[ N_MAX ];
int demands[ N_MAX ];
bool seen[ N_MAX ];
float costs[ N_MAX ][ N_MAX ]; /* distance from node i to node j */

int main() {
  int k, /* number of trucks */
      n, /* number of customers */
      c; /* capacity of each truck */

  string lables; /* text labels such as NODE_COORD_SECTION which are useless */
  cin >> k >> n >> c;

  if(n > N_MAX) {
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

  /* caclulate costs */
  rep(i,0,n)
    rep(j, i+1, n)
      costs[i][j] = costs[j][i] = sqrt( (xs[i]-xs[j])*(xs[i]-xs[j]) + (ys[i]-ys[j])*(ys[i]-ys[j]) );

  int ans = 0;
  seen[0] = true;
  int cnt = 1; // how many people we have seen

  while(cnt < n) {
    int cur = 0;
    int rem = c;
    while(rem) {
      int nearest = -1;
      rep(i,1,n)
        if(!seen[i] && (rem > demands[i]) && (nearest == -1 || costs[cur][i] < costs[cur][nearest]) )
          nearest = i;
      if(nearest == -1) break;
      seen[nearest] = true;
      cnt += 1;
      rem = rem - demands[nearest];
      ans += costs[cur][nearest];
      cur = nearest;
    }
    ans += costs[cur][0];
  }
  cout << "result ==> " << ans << endl;

  return 0;
}
