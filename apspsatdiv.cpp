#include <iostream>
#include <algorithm>
#include <cstdio>
#include <vector>
using namespace std ;
typedef long long ll ;
/*
 *   Encode the shortest average distance on regular graphs as a SAT problem.
 */
int n, k ;
const int MAXN = 64 ;
const int MAXD = 4 ;
// dvar[1][i][j] for i<j are the primary input variables
int dvar[MAXD][MAXN][MAXN] ;
// tvar[d][i][j][k] is temp var from dvar[d-1][i][j] and dvar[1][j][k]
// (modulo ordering)
int tvar[MAXD][MAXN][MAXN][MAXN] ;
const int FALSE = 1000000000 ;
const int TRUE = -FALSE ;
void error(const char *s) {
   cerr << s << endl ;
   exit(10) ;
}
int varn = 1 ;
int makevar() {
   return varn++ ;
}
int neg(int v) {
   return -v ;
}
int truity(int v) {
   return (v == TRUE) ;
}
int notfalsity(int v) {
   return (v != FALSE) && (v != 0) ;
}
int bc(int v) {
   int r = 1 ;
   while (v >= (1 << r))
      r++ ;
   return r ;
}
void emit(const vector<int> &v) {
   for (auto it=v.begin(); it != v.end(); it++)
      if (truity(*it))
         return ;
   for (auto it=v.begin(); it != v.end(); it++)
      if (notfalsity(*it))
         printf(" %d", *it) ;
   printf(" 0\n") ;
}
void emit(int a) {
   emit(vector<int>({a})) ;
}
void emit(int a, int b) {
   emit(vector<int>({a,b})) ;
}
void emit(int a, int b, int c) {
   emit(vector<int>({a,b,c})) ;
}
void emit(int a, int b, int c, int d) {
   emit(vector<int>({a,b,c,d})) ;
}
void twosort(int a, int b, int c, int d) {
   emit(neg(a), c) ;
   emit(neg(b), c) ;
   emit(neg(a), neg(b), d) ;
}
int checkzero(int k, int i, int j, int delta) {
   if (j == i)
      return makevar() ;
   k = (k & ((1 << i) - (1 << j))) ;
   if (k <= delta)
      return makevar() ;
   return FALSE ;
}
void counts(vector<int> &tsi, int delta) {
   if (tsi.size() > 0) {
      vector<int> tosum ;
      for (auto v : tsi)
         tosum.push_back(v) ;
      for (int i=1; tosum.size() > (1<<(i-1)); i++) {
         for (int j=i; j>0; j--) {
            vector<int> b = tosum ;
            if (i == j) {
               for (int k=0; k<(int)tosum.size(); k++) {
                  int m = (k ^ ((1 << j) - 1)) ;
                  if (m > k && m < (int)tosum.size()) {
                     b[k] = checkzero(k, i, j, delta) ;
                     b[m] = checkzero(m, i, j, delta) ;
                     twosort(tosum[k], tosum[m], b[k], b[m]) ;
                  }
               }
            } else {
               for (int k=0; k<(int)tosum.size(); k++) {
                  int m = (k ^ (1 << (j - 1))) ;
                  if (m > k && m < (int)tosum.size()) {
                     b[k] = checkzero(k, i, j, delta) ;
                     b[m] = checkzero(m, i, j, delta) ;
                     twosort(tosum[k], tosum[m], b[k], b[m]) ;
                  }
               }
            }
            tosum = b ;
         }
         if ((1 << (i - 1)) > delta) {
            vector<int> newsum ;
            for (int k=0; k<(int)tosum.size(); k++) {
               if ((k & (1 << (i - 1))) == 0) {
                  newsum.push_back(tosum[k]) ;
               }
            }
            tosum = newsum ;
            i-- ;
         }
      }
      emit(neg(tosum[delta])) ;
   }
}
int mod ;
int findvar(int d, int i, int j) {
   if (i > j)
      swap(i, j) ;
   int im = i % mod ;
   int jm = j % mod ;
   if (im > jm) {
      i += jm-im ;
      j -= jm-im ;
   }
   return dvar[d][i][j] ;
}
int main(int argc, char *argv[]) {
   while (argc > 3 && argv[1][0] == '-') {
      argc-- ;
      argv++ ;
      switch (argv[0][1]) {
default: error("! bad argument") ;
      }
   }
   n = atol(argv[1]) ;
   k = atol(argv[2]) ;
   int m = atol(argv[3]) ;
   if (n % m != 0)
      error("! not doing it . . .") ;
   mod = n / m ;
   printf("p cnf 1 1\n") ;
   // we build the level 1 and level 2 variables
   for (int d=1; d<=2; d++)
      for (int i=0; i<n; i++)
         for (int j=i+1; j<n; j++)
            if (i < mod && i % mod <= j % mod)
               dvar[d][i][j] = makevar() ;
            else
               dvar[d][i][j] = dvar[d][i%mod][i%mod-i+j] ;
   // we build the ijk variables
   for (int i=0; i<n; i++)
      for (int j=i+1; j<n; j++) {
         vector<int> vars ;
         for (int k=0; k<n; k++)
            if (k != i && k != j) {
               tvar[2][i][k][j] = makevar() ;
               emit(findvar(1, i, k), neg(tvar[2][i][k][j])) ;
               emit(findvar(1, k, j), neg(tvar[2][i][k][j])) ;
               vars.push_back(tvar[2][i][k][j]) ;
            }
         vars.push_back(neg(dvar[2][i][j])) ;
         emit(vars) ;
      }
   // require the degree of each node to be less than k
   for (int i=0; i<n; i++) {
      vector<int> vars ;
      for (int j=0; j<n; j++)
         if (i != j)
            vars.push_back(findvar(1, i, j)) ;
      counts(vars, k) ;
   }
   // say all pairs need to have one set (for now).
   for (int i=0; i<n; i++)
      for (int j=i+1; j<n; j++)
         emit(findvar(1, i, j), findvar(2, i, j)) ;
}
