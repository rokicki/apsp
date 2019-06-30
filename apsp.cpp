/*
 *   Read graphs and calculate the diameter and average shortest
 *   path length.
 */
#include <iostream>
#include <cstdio>
#include <inttypes.h>
using namespace std ;
const int MAXBUF = 1000 ;
char mbuf[MAXBUF+1] ;
int n ;
const int MAXN = 128 ;
const int INF = MAXN ;
int adj[MAXN][MAXN] ;
int e ;
double goal = 0 ;
int k ;
void newgraph() {
   n = 0 ;
   e = 0 ;
   goal = 0 ;
}
void addedge(int a, int b) {
   while (a >= n || b >= n) {
      for (int i=0; i<n; i++)
         adj[i][n] = adj[n][i] = INF ;
      adj[n][n] = 0 ;
      n++ ;
   }
   if (adj[a][b] == INF)
      e++ ;
   adj[a][b] = adj[b][a] = 1 ;
}
double best = 1000 ;
long long processed = 0 ;
const int maxcount = 1000 ;
int cnt = 0 ;
int processgraph() {
   processed++ ;
   if (goal == 0) {
      if (2 * e % n != 0)
         printf("Not a regular graph?\n") ;
      k = 2 * e / n ;
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
   }
   for (int k=0; k<n; k++)
      for (int i=0; i<n; i++)
         for (int j=0; j<n; j++)
            if (adj[i][k] + adj[k][j] < adj[i][j])
               adj[i][j] = adj[i][k] + adj[k][j] ;
   int hi = 0 ;
   int sum = 0 ;
   for (int i=0; i<n; i++)
      for (int j=0; j<n; j++) {
         hi = max(hi, adj[i][j]) ;
         sum += adj[i][j] ;
      }
   double avg = sum / (double)(n * (n-1)) ;
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
      fflush(stdout) ;
      for (int i=0; i<n; i++) {
         printf("%d", i) ;
         for (int j=i+1; j<n; j++)
            if (adj[i][j] == 1)
               printf(" %d", j) ;
         printf("\n") ;
      }
   }
   return 0 ;
}
int main() {
   bool ingraph = 0 ;
   bool early = 0 ;
   while (fgets(mbuf, MAXBUF, stdin)) {
      char *p = mbuf ;
      while (*p && *p <= ' ')
         p++ ;
      if (*p == 'G') {
         ingraph = 1 ;
         newgraph() ;
      } else if ((*p == 0 && ingraph) || *p == 'T') {
         ingraph = 0 ;
         if (processgraph()) {
            early = 1 ;
            break ;
         }
      } else if (ingraph && *p >= '0' && *p <= '9') {
         int a = strtol(p, &p, 10) ;
         while (*p && (*p < '0' || *p > '9'))
            p++ ;
         while (1) {
            int b = strtol(p, &p, 10) ;
            if (b == 0)
               break ;
            addedge(a-1, b-1) ;
         }
      } else {
         // ignore this line
      }
   }
   if (ingraph) {
      processgraph() ;
   }
   if (early)
      cout << "Processed " << processed << "(early) n " << n << " k " << k << " best " << best << " opt " << goal << endl ;
   else
      cout << "Processed " << processed << " n " << n << " k " << k << " best " << best << " opt " << goal << endl ;
}
/*
Graph 1:

1 : 2 3 4 5 
2 : 1 3 4 5 
3 : 1 2 6 7 
4 : 1 2 6 8 
5 : 1 2 7 8 
6 : 3 4 7 8 
7 : 3 5 6 8 
8 : 4 5 6 7 
Taillenweite: 3

4 : 1 2 3 5 4 7 6 8
3 : 1 2 4 3 5 6 8 7
3 : 1 2 5 4 3 8 7 6
1 : 2 1 3 4 5 6 7 8
Ordnung: 12

*/
