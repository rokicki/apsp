/*
 *   Read graphs.  Template for things that do work.
 */
#include <iostream>
#include <cstdio>
#include <inttypes.h>
#include <vector>
#include "ClpSimplex.hpp"
#include "ClpPresolve.hpp"
#include <cstdio>
using namespace std ;
const double eps = 1e-6 ;
const int MAXBUF = 16000 ;
char mbuf[MAXBUF+1] ;
int n ;
const int MAXN = 128 ;
const int INF = MAXN ;
double bestflow = 1e9 ;
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
double dolp(const char *fn) {
  ClpSimplex  model;
  model.setLogLevel(0) ;
  int status;
  status=model.readLp(fn) ;
  double res = -1 ;
  if (!status) {
   ClpPresolve presolveInfo; 
   ClpSimplex * presolvedModel = presolveInfo.presolvedModel(model); 
  // at this point we have original model and a new model.  The  information
  // on the operations done is in presolveInfo 
   if (presolvedModel) { 
    // was not found to be infeasible - so lets solve 
    // if presolvedModel was NULL then it was primal infeasible and ... 
     presolvedModel->setPerturbation(100) ;
     presolvedModel->primal(); // or whatever else we wish to do 
//   presolveInfo.postsolve(true);  // the true updates status arrays in original       
     res = presolvedModel->objectiveValue() ;
     delete presolvedModel ;
   } else {
     model.primal() ;
     res = model.objectiveValue() ;
   }
  }
  return res ;
} 
void processgraph() {
   int k = 2 * e / n ;
   double optsum = k ;
   int left = n - 1 - k ;
   int d = 2 ;
   int cur = k ;
   while (left) {
      cur *= k-1 ;
      int thislev = min(left, cur) ;
      left -= thislev ;
      optsum += thislev * d ;
      d++ ;
   }
   double goalpbar = optsum / (double)(n-1) ;
   double goalt = optsum*n / (2.0*e) ;
   apsp() ;
   int dia = 1 ;
   int sum = 0 ;
   for (int i=0; i<n; i++)
      for (int j=0; j<n; j++) {
         sum += adj[i][j] ;
         dia = max(dia, adj[i][j]) ;
         totflow[i][j] = 0 ;
      }
   double pbar = sum / (double)(n*(n-1)) ;
   double xflow = sum /(2.0*e) ;
// if (xflow >= bestflow - eps) {
//    newgraph() ;
//    return ;
// }
   int pid = getpid() ;
   char fbuf[100] ;
   sprintf(fbuf, "t%d.lp", pid) ;
   FILE *f = fopen(fbuf, "w") ;
   if (f == 0)
      error("! can't open output file") ;
   fprintf(f, "min g\nst\n") ;
   for (int i=0; i<n; i++)
      for (int d=0; d<n; d++) { // flow through node i to destination d
         for (int j=0; j<n; j++) {
            if (adj[i][j] == 1) {
               if (adj[i][d] == adj[j][d] + 1) {
                  fprintf(f, " - f_%d_%d_%d", i, j, d) ;
               } else if (adj[i][d] + 1 == adj[j][d]) {
                  fprintf(f, " + f_%d_%d_%d", j, i, d) ;
               }
            }
         }
         if (i == d) {
            fprintf(f, " = %d\n", n-1) ;
         } else {
            fprintf(f, " = -1\n") ;
         }
      }
   for (int i=0; i<n; i++)
      for (int j=0; j<n; j++) {
         if (adj[i][j] == 1) {
            for (int d=0; d<n; d++)
               if (adj[i][d] == adj[j][d] + 1)
                  fprintf(f, " + f_%d_%d_%d", i, j, d) ;
            fprintf(f, " - g <= 0\n") ;
         }
      }
   fprintf(f, "end\n") ;
   fclose(f) ;
   double res = dolp(fbuf) ;
   remove(fbuf) ;
   if (res < bestflow)
      bestflow = res ;
   cout << "N " << n << " k " << k << " pbar " << pbar << " f " << goalpbar/pbar << " gs " << res << " f " << goalt/res << endl ;
   if (res <= goalt + eps)
      exit(0) ;
   if (goalt/res >= goalpbar / pbar - eps)
      exit(0) ;
   newgraph() ;
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
         if (strcmp(argv[0], "-g6") == 0) {
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
