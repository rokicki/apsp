/*
 *   Read graphs.  Template for things that do work.
 */
#include <iostream>
#include <cstdio>
#include <inttypes.h>
#include <vector>
using namespace std ;
const double eps = 1e-8 ;
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
void apsp() {
   for (int k=0; k<n; k++)
      for (int i=0; i<n; i++)
         for (int j=0; j<n; j++)
            if (adj[i][k] + adj[k][j] < adj[i][j])
               adj[i][j] = adj[i][k] + adj[k][j] ;
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
int bydist[MAXN] ;
int cnts[MAXN] ;
double totflow[MAXN][MAXN] ;
double flowat[MAXN][MAXN] ;
void processgraph() {
   int k = 2 * e / n ;
   apsp() ;
   cout << "min g" << endl ;
   cout << "st" << endl ;
   for (int i=0; i<n; i++)
      for (int d=0; d<n; d++) { // flow through node i to destination d
         for (int j=0; j<n; j++) {
            if (adj[i][j] == 1) {
               if (adj[i][d] == adj[j][d] + 1) {
                  cout << " - " ;
                  cout << "f_" << i << "_" << j << "_" << d ;
               } else if (adj[i][d] + 1 == adj[j][d]) {
                  cout << " + " ;
                  cout << "f_" << j << "_" << i << "_" << d ;
               }
            }
         }
         if (i == d) {
            cout << " = " << (n-1) << endl ;
         } else {
            cout << " = -1" << endl ;
         }
      }
   for (int i=0; i<n; i++)
      for (int j=0; j<n; j++) {
         if (adj[i][j] == 1) {
            for (int d=0; d<n; d++)
               if (adj[i][d] == adj[j][d] + 1)
                  cout << " + f_" << i << "_" << j << "_" << d ;
            cout << " - g <= 0" << endl ;
         }
      }
   cout << "end" << endl ;
   exit(0) ;
// newgraph() ;
}
void dog6() {
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
void dogenreg() {
   bool ingraph = 0 ;
   while (fgets(mbuf, MAXBUF, stdin)) {
      char *p = mbuf ;
      while (*p && *p <= ' ')
         p++ ;
      if (*p == 'G') {
         ingraph = 1 ;
         newgraph() ;
      } else if (*p == 'T') {
         ingraph = 0 ;
         processgraph() ;
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
   if (ingraph)
      processgraph() ;
}
int useg6 = 0 ;
int main(int argc, char *argv[]) {
   while (argc > 1 && argv[1][0] == '-') {
      argc-- ;
      argv++ ;
      switch(argv[0][1]) {
case 'g':
         if (strcmp(argv[0], "-g6") == 1) {
            useg6 = 1 ;
            break ;
         }
default:
         error("! bad argument") ;
      }
   }
   if (useg6)
      dog6() ;
   else
      dogenreg() ;
}
