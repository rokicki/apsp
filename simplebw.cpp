/*
 *   Read graphs.  Template for things that do work.
 */
#include <iostream>
#include <cstdio>
#include <inttypes.h>
#include <vector>
using namespace std ;
const int MAXBUF = 16000 ;
char mbuf[MAXBUF+1] ;
int n ;
const int MAXN = 128 ;
const int INF = MAXN ;
int adj[MAXN][MAXN] ;
int e ;
int k ;
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
   apsp() ;
   int dia = 1 ;
   int sum = 0 ;
   for (int i=0; i<n; i++)
      for (int j=0; j<n; j++) {
         sum += adj[i][j] ;
         dia = max(dia, adj[i][j]) ;
         totflow[i][j] = 0 ;
      }
   double xflow = sum / (2.0 * e) ;
   double curflow[MAXN] ;
   for (int src=0; src<n; src++) {
      for (int d=0; d<dia+2; d++)
         cnts[d] = 0 ;
      for (int dst=0; dst<n; dst++)
         cnts[1+adj[src][dst]]++ ;
      for (int d=1; d<=dia; d++)
         cnts[d] += cnts[d-1] ;
      for (int dst=0; dst<n; dst++)
         bydist[cnts[adj[src][dst]]++] = dst ;
      vector<pair<int, int> > edges ;
      for (int d=0; d<dia; d++)
         for (int i=cnts[d]; i<cnts[d+1]; i++)
            for (int j=cnts[d+1]; j<cnts[d+2]; j++)
               if (adj[i][j] == 1)
                  edges.push_back({i,j}) ;
      for (int i=0; i<n; i++)
         for (int j=0; j<n; j++)
            flowat[i][j] = 0 ;
      for (int i=0; i<n; i++)
         flowat[src][i] = (i == src ? 0 : 1) ;
      for (int vi=0; vi<n; vi++) {
         int a = bydist[vi] ;
         int splitby[MAXN] ;
         for (int i=0; i<n; i++)
            splitby[i] = 0 ;
         for (int b=0; b<n; b++)
            if (adj[a][b] == 1 && adj[src][b] == adj[src][a] + 1)
               for (int d=0; d<n; d++)
                  if (adj[src][d] == adj[src][a] + 1 + adj[b][d])
                     splitby[d]++ ;
         for (int b=0; b<n; b++)
            if (adj[a][b] == 1 && adj[src][b] == adj[src][a] + 1)
               for (int d=0; d<n; d++) {
                  if (adj[src][d] == adj[src][a] + 1 + adj[b][d]) {
                     double tflow = flowat[a][d] / splitby[d] ;
                     flowat[b][d] += tflow ;
                     totflow[a][b] += tflow ;
                  }
               }
      }
   }
   double hiflow = 0 ;
   for (int i=0; i<n; i++)
      for (int j=i+1; j<n; j++)
         if (adj[i][j] == 1)
            hiflow = max(hiflow, totflow[j][i]) ;
   cout << "Flow: " << hiflow << " frac " << xflow/hiflow << endl ;
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
