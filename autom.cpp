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
   cout << "n=" << n << endl ;
   cout << 'g' << endl ;
   for (int i=0; i<n; i++) {
      for (int j=0; j<i; j++)
         if (adj[i][j] == 1)
            cout << j << "," ;
      cout << ";" << endl ;
   }
   cout << "x" << endl ;
   return 0 ;
}
int main(int argc, char *argv[]) {
   bool ingraph = 0 ;
   bool early = 0 ;
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
}
