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
int goalk ;
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
      if (2 * e % n != 0) {
         printf("Not a regular graph? e %d n %d\n", e, n) ;
         return 0 ;
      }
      k = 2 * e / n ;
      if (k != goalk)
         return 0 ;
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
   if (hi == INF) {
      return 0 ;
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
         printf("Order %d degree %d diameter %d avg %g *\n", n, k, hi, avg) ;
      else
         printf("Order %d degree %d diameter %d avg %g\n", n, k, hi, avg) ;
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
int main(int argc, char *argv[]) {
   bool ingraph = 0 ;
   bool early = 0 ;
   goalk = atol(argv[1]) ;
   while (fgets(mbuf, MAXBUF, stdin)) {
      newgraph() ;
      int len = strlen(mbuf) ;
      while (len > 0 && mbuf[len-1] <= ' ')
         len-- ;
      int localn = mbuf[0] - 63 ;
      int off = 1 ;
      if (mbuf[0] == 126) {
         localn = ((mbuf[1] - 63) << 12) + ((mbuf[2] - 63) << 6) + mbuf[3] - 63 ;
         off = 4 ;
      }
      int xlen = off + ((localn * (localn-1) / 2) + 5) / 6 ;
      if (xlen != len) {
         printf("Failed reading a graph\n") ;
         break ;
      }
      int at = 0 ;
      for (int i=0; i<localn; i++) {
         for (int j=0; j<i; j++) {
            if (((mbuf[off+at/6] - 63) >> (5 - at % 6)) & 1)
               addedge(i, j) ;
            at++ ;
         }
      }
      if (e)
         processgraph() ;
   }
   if (early)
      cout << "Processed " << processed << "(early) n " << n << " k " << k << " best " << best << " opt " << goal << endl ;
   else
      cout << "Processed " << processed << " n " << n << " k " << k << " best " << best << " opt " << goal << endl ;
}
