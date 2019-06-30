/*
 *   Read graphs and calculate the diameter and average shortest
 *   path length.
 */
#include <iostream>
#include <vector>
#include <cstdio>
#include <inttypes.h>
#include <sys/time.h>
using namespace std ;
static double start ;
double walltime() {
   struct timeval tv ;
   gettimeofday(&tv, 0) ;
   return tv.tv_sec + 0.000001 * tv.tv_usec ;
}
double duration() {
   double now = walltime() ;
   double r = now - start ;
   start = now ;
   return r ;
}
const int MAXBUF = 1000 ;
char mbuf[MAXBUF+1] ;
int n ;
const int MAXN = 127 ;
const int INF = MAXN ;
int adj[MAXN][MAXN] ;
int v[MAXN][MAXN] ;
int e ;
double goal = 0 ;
int k ;
void newgraph() {
   n = 0 ;
   e = 0 ;
   goal = 0 ;
}
vector<pair<int, int> > edges ;
void addedge(int a, int b) {
   v[a][b] = v[b][a] = 1 ;
   edges.push_back({a,b}) ;
}
double best = 1000 ;
long long processed = 0 ;
const int maxcount = 100 ;
double lastavg ;
int cnt = 0 ;
void apsp() {
   for (int k=0; k<n; k++)
      for (int i=0; i<n; i++)
         for (int j=0; j<n; j++)
            if (adj[i][k] + adj[k][j] < adj[i][j])
               adj[i][j] = adj[i][k] + adj[k][j] ;
}
int processgraph() {
   processed++ ;
   for (int i=0; i<n; i++)
      for (int j=i+1; j<n; j++)
         if (v[i][j] == 1)
            adj[i][j] = adj[j][i] = 1 ;
         else
            adj[i][j] = adj[j][i] = INF ;
   apsp() ;
   int hi = 0 ;
   int sum = 0 ;
   for (int i=0; i<n; i++)
      for (int j=0; j<n; j++) {
         hi = max(hi, (int)adj[i][j]) ;
         sum += adj[i][j] ;
      }
   double avg = sum / (double)(n * (n-1)) ;
   lastavg = avg ;
   if (avg <= best) {
      if (avg == best) {
         cnt++ ;
         if (cnt > maxcount) {
            if (cnt == maxcount + 1) {
               printf("Skipping more equivalent solutions.\n") ;
               fflush(stdout) ;
            }
            return best == goal ;
         }
      } else {
         cnt = 1 ;
      }
      best = avg ;
      if (best == goal)
         printf("Order %d diameter %d avg %g *\n", n, hi, avg) ;
      else
         printf("Order %d diameter %d avg %g\n", n, hi, avg) ;
      for (int i=0; i<n; i++) {
         printf("%d", i) ;
         for (int j=i+1; j<n; j++)
            if (adj[i][j] == 1)
               printf(" %d", j) ;
         printf("\n") ;
      }
      fflush(stdout) ;
   }
   return 0 ;
}
int lasti, lastj ;
void doswap(int i, int j) {
   lasti = i ;
   lastj = j ;
   v[edges[i].first][edges[i].second] = v[edges[i].second][edges[i].first] =
   v[edges[j].first][edges[j].second] = v[edges[j].second][edges[j].first] = 0 ;
   v[edges[i].first][edges[j].second] = v[edges[j].second][edges[i].first] =
   v[edges[j].first][edges[i].second] = v[edges[i].second][edges[j].first] = 1 ;
   swap(edges[i].second, edges[j].second) ;
}
void swaptwo() {
   int ec = edges.size() ;
   while (1) {
      int i = (int)(ec*drand48()) ;
      swap(edges[i].first, edges[i].second) ;
      int j = (int)(ec*drand48()) ;
      while (i == j)
         j = (int)(ec*drand48()) ;
      if (edges[i].first == edges[j].first ||
          edges[i].first == edges[j].second ||
          edges[i].second == edges[j].first ||
          edges[i].second == edges[j].second ||
          v[edges[i].first][edges[j].second] == 1 ||
          v[edges[i].second][edges[j].first] == 1)
         continue ;
      doswap(i, j) ;
      return ;
   }
}
int main(int argc, char *argv[]) {
   start = walltime() ;
   bool ingraph = 0 ;
   bool early = 0 ;
   n = atol(argv[1]) ;
   k = atol(argv[2]) ;
   int left = n - 1 ;
   int sum = 0 ;
   int poss = k ;
   int d = 1 ;
   while (left > 0) {
      int t = min(left, poss) ;
      left -= t ;
      sum += t * d ;
      d++ ;
      poss = poss * (k - 1) ;
   }
   goal = sum / (double)(n - 1) ;
   left = k * n / 2 ;
   for (int i=0; left && i<n; i++) {
      if (left >= n) {
         for (int j=0; j<n; j++)
            addedge(j, (j+i+1)%n) ;
         left -= n ;
      } else {
         if ((i & 1) == 1)
            continue ;
         for (int j=0; j<n; j += 2)
            addedge(j, (j+i+1)%n) ;
         left -= n/2 ;
      }
   }
   processgraph() ;
   double curr = lastavg ;
   swaptwo() ;
   double frac = 1 ;
   while (1) {
      if (processgraph())
         break ;
      if (lastavg <= curr || drand48() < frac) {
         curr = lastavg ;
      } else {
         doswap(lasti, lastj) ;
         lastavg = curr ;
      }
      swaptwo() ;
      if ((processed & 65535) == 0) {
         frac *= 0.991 ;
         if (frac < 1e-10)
            break ;
      }
      if ((processed & (processed - 1)) == 0)
         cout << "Processed " << processed << " in " << walltime()-start << " frac " << frac << endl << flush ;
   }
}
