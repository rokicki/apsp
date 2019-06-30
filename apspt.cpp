/*
 *   Read graphs and calculate the diameter and average shortest
 *   path length.
 */
#include <iostream>
#include <vector>
#include <cstring>
#include <strings.h>
#include <cstdio>
#include <cstdlib>
#include <inttypes.h>
#include <sys/time.h>
using namespace std ;
typedef unsigned long long ull ;
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
int e ;
int n ;
const int MAXN = 64 ;
const int INF = MAXN ;
ull bv[MAXN] ;
/* assumes bv is valid */
int diameter ;
double fastapsp() {
   ull seen[MAXN] ;
   ull dodd[MAXN] ;
   ull deven[MAXN] ;
   for (int i=0; i<n; i++)
      seen[i] = (1LL << i) | bv[i] ;
   int cnt = n ;
   int d = 1 ;
   int left = n * n - n - 2 * e ;
   int sum = 2 * e + left * 2 ;
   for (int i=0; i<n; i++)
      dodd[i] = bv[i] ;
   if (left)
      d++ ;
   while (left) {
      int oleft = left ;
      for (int i=0; i<n; i++) {
         ull r = 0 ;
         ull m = bv[i] ;
         while (m) {
            int b = ffsll(m)-1 ;
            m ^= 1LL << b ;
            r |= dodd[b] ;
         }
         deven[i] = r ;
         ull nw = r & ~seen[i] ;
         seen[i] |= r ;
         left -= __builtin_popcountll(nw) ;
      }
      if (left == 0)
         break ;
      d++ ;
      sum += left ;
      for (int i=0; i<n; i++) {
         ull r = 0 ;
         ull m = bv[i] ;
         while (m) {
            int b = ffsll(m)-1 ;
            m ^= 1LL << b ;
            r |= deven[b] ;
         }
         dodd[i] = r ;
         ull nw = r & ~seen[i] ;
         seen[i] |= r ;
         left -= __builtin_popcountll(nw) ;
      }
      if (left == oleft)
         return 1e20 ;
      if (left == 0)
         break ;
      sum += left ;
      d++ ;
   }
   diameter = d ;
   return sum / (double)(n*(n-1)) ;
}
double goal = 0 ;
int k ;
vector<pair<int, int> > edges ;
void addedge(int a, int b) {
   bv[a] |= 1LL << b ;
   bv[b] |= 1LL << a ;
   edges.push_back({a,b}) ;
}
double best = 1000 ;
long long processed = 0 ;
const int maxcount = 100 ;
double lastavg ;
int cnt = 0 ;
double pr = 0 ;
long long countlim ;
int processgraph() {
   processed++ ;
   e = (int)edges.size() ;
   double avg = fastapsp() ;
   int hi = diameter ;
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
         printf("Order %d diameter %d avg %g * %g\n", n, hi, avg, pr) ;
      else
         printf("Order %d diameter %d avg %g %g\n", n, hi, avg, pr) ;
      for (int i=0; i<n; i++) {
         printf("%d", i) ;
         for (int j=i+1; j<n; j++)
            if ((bv[i] >> j) & 1)
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
   bv[edges[i].first] &= ~(1LL << edges[i].second) ;
   bv[edges[i].second] &= ~(1LL << edges[i].first) ;
   bv[edges[j].first] &= ~(1LL << edges[j].second) ;
   bv[edges[j].second] &= ~(1LL << edges[j].first) ;
   swap(edges[i].second, edges[j].second) ;
   bv[edges[i].first] |= (1LL << edges[i].second) ;
   bv[edges[i].second] |= (1LL << edges[i].first) ;
   bv[edges[j].first] |= (1LL << edges[j].second) ;
   bv[edges[j].second] |= (1LL << edges[j].first) ;
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
          ((bv[edges[i].first] >> edges[j].second) & 1) ||
          ((bv[edges[i].second] >> edges[j].first) & 1))
         continue ;
      doswap(i, j) ;
      return ;
   }
}
double multiplier = 0.99 ;
int main(int argc, char *argv[]) {
   while (argc > 1 && argv[1][0] == '-') {
      argc-- ;
      argv++ ;
      switch (argv[0][1]) {
case 'm':
         multiplier = atof(argv[1]) ;
         argc-- ;
         argv++ ;
         break ;
default:
         cout << "Bad argument " << argv[0] << endl ;
         exit(0) ;
      }
   }
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
   double frac = 0.001 ;
   pr = frac ;
   countlim = 1000 ;
   long long countdown = countlim ;
   while (1) {
      if (processgraph())
         break ;
      if (lastavg <= curr || drand48() < frac) {
         curr = lastavg ;
      } else if (0 && countdown-- <= 0) {
         curr = lastavg ;
         countlim += countlim / 10 ;
         if (countlim > e*e*2)
            countlim = e*e*2 ;
         countdown = countlim ;
      } else {
         doswap(lasti, lastj) ;
         lastavg = curr ;
      }
      swaptwo() ;
      if ((processed & 65535) == 0) {
         frac *= multiplier ;
         if (frac < 1e-6)
            frac = 1e-6 ;
         if (frac < 1e-10)
            break ;
         pr = frac ;
      }
      if ((processed & (processed - 1)) == 0)
         cout << "Processed " << processed << " in " << walltime()-start << " frac " << frac << endl << flush ;
   }
}
