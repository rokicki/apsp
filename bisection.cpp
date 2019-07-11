/*
 *   Calculate the bisection width of a graph.  Slow.
 */
#include <iostream>
#include <cstdio>
#include <inttypes.h>
using namespace std ;
const int MAXBUF = 16000 ;
char mbuf[MAXBUF+1] ;
int n ;
const int MAXN = 128 ;
const int INF = MAXN ;
int adj[MAXN][MAXN] ;
int e ;
int k ;
void error(const char *s) {
   cerr << s << endl ;
   exit(10) ;
}
void newgraph() {
   n = 0 ;
   e = 0 ;
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
int hi ;
int inset[MAXN] ;
void recur(int at, int need, int have, int sofar) {
   if (need == 0) {
      hi = max(hi, sofar) ;
      return ;
   }
   if (n - at > need)
      recur(at+1, need, have, sofar) ;
   for (int i=0; i<have; i++)
      if (adj[at][inset[i]] == 1)
         sofar++ ;
   inset[have] = at ;
   recur(at+1, need-1, have+1, sofar) ;
}
void processgraph() {
   if (e * 2 % n != 0)
      error("! not regular?") ;
   k = e * 2 / n ;
   int need = n >> 1 ;
   hi = 0 ;
   recur(0, need, 0, 0) ;
   cout << "Cut is " << ((need * k) - 2 * hi) << endl ;
   newgraph() ;
}
int main(int argc, char *argv[]) {
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
         printf("Failed reading a graph xlen %d len %d localn %d\n", xlen, len, localn) ;
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
}
