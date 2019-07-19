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
#ifdef BIG
typedef __uint128_t ull ;
#else
typedef unsigned long long ull ;
#endif
static double start ;
static double pstart ;
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
void error(const char *s) {
   cerr << s << endl ;
   exit(10) ;
}
const int MAXBUF = 1000 ;
char mbuf[MAXBUF+1] ;
int e ;
int n ;
int mod, ddiv ;
#ifdef BIG
const int MAXN = 128 ;
ull onebit[MAXN] ;
#define BIT(i) onebit[i]
#else
const int MAXN = 64 ;
#define BIT(i) (1LL<<(i))
#endif
const int INF = MAXN ;
ull bv[MAXN] ;
ull sbv[MAXN] ;
/* assumes bv is valid */
int diameter ;
int bigpopcount(ull v) {
#ifdef BIG
   unsigned long long t1 = v ;
   unsigned long long t2 = (v >> 64) ;
   return __builtin_popcountll(t1) + __builtin_popcountll(t2) ;
#else
   return __builtin_popcountll(v) ;
#endif
}
int bigffs(ull v) {
#ifdef BIG
   unsigned long long t1 = v ;
   if (t1)
      return ffsll(t1) ;
   return 64 + ffsll(v >> 64) ;
#else
   return ffsll(v) ;
#endif
}
int bc(ull v) {
   return bigpopcount(v) ;
}
double fastapsp() {
   ull seen[MAXN] ;
   ull dodd[MAXN] ;
   ull deven[MAXN] ;
   for (int i=0; i<n; i++)
      seen[i] = BIT(i) | bv[i] ;
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
            int b = bigffs(m)-1 ;
            m ^= BIT(b) ;
            r |= dodd[b] ;
         }
         deven[i] = r ;
         ull nw = r & ~seen[i] ;
         seen[i] |= r ;
         left -= bigpopcount(nw) ;
      }
      if (left == 0)
         break ;
      d++ ;
      sum += left ;
      for (int i=0; i<n; i++) {
         ull r = 0 ;
         ull m = bv[i] ;
         while (m) {
            int b = bigffs(m)-1 ;
            m ^= BIT(b) ;
            r |= deven[b] ;
         }
         dodd[i] = r ;
         ull nw = r & ~seen[i] ;
         seen[i] |= r ;
         left -= bigpopcount(nw) ;
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
void addedge(int aa, int bb) {
   if ((bv[aa] >> bb) & 1)
      return ;
   for (int p=0; p<n; p += mod) {
      int a = (aa + p) % n ;
      int b = (bb + p) % n ;
      bv[a] |= BIT(b) ;
      bv[b] |= BIT(a) ;
   }
   edges.push_back({aa,bb}) ;
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
   e = ddiv*(int)edges.size() ;
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
int lastip, lastjp ;
int doswap(int ii, int jj, int ip, int jp) {
   for (int i=0; i<n; i++)
      sbv[i] = bv[i] ;
   for (int p=0; p<n; p+=mod) {
      int i = (edges[ii].first + p) % n ;
      int j = (edges[ii].second + p) % n ;
      if (0 == ((bv[i] >> j) & 1)) {
// cout << "Fail 1" << endl ;
         goto unwindit2 ;
      }
      bv[i] &= ~(BIT(j)) ;
      bv[j] &= ~(BIT(i)) ;
      i = (edges[jj].first + p) % n ;
      j = (edges[jj].second + p) % n ;
      if (0 == ((bv[i] >> j) & 1)) {
// cout << "Fail 2" << endl ;
         goto unwindit2 ;
      }
      bv[i] &= ~(BIT(j)) ;
      bv[j] &= ~(BIT(i)) ;
   }
   swap(edges[ii].second, edges[jj].second) ;
   edges[ii].second = (edges[ii].second + ip) % n ;
   edges[jj].second = (edges[jj].second + jp) % n ;
   for (int p=0; p<n; p+=mod) {
      int i = (edges[ii].first + p) % n ;
      int j = (edges[ii].second + p) % n ;
      if (i == j || 1 == ((bv[i] >> j) & 1)) {
// cout << "Fail 3 " << " " << i << " " << j << " " << ip << " " << jp << endl ;
         goto unwindit ;
      }
      bv[i] |= (BIT(j)) ;
      bv[j] |= (BIT(i)) ;
      i = (edges[jj].first + p) % n ;
      j = (edges[jj].second + p) % n ;
      if (i == j || 1 == ((bv[i] >> j) & 1)) {
// cout << "Fail 4 " << " " << i << " " << j << " " << ip << " " << jp << endl ;
         goto unwindit ;
      }
      bv[i] |= (BIT(j)) ;
      bv[j] |= (BIT(i)) ;
   }
   lasti = ii ;
   lastj = jj ;
   lastip = ip ;
   lastjp = jp ;
   for (int i=0; i<n; i++)
      if (bc(bv[i]) != k) {
         error("! bad state") ;
      }
   return 1 ;
unwindit:
   edges[ii].second = (edges[ii].second + n - ip) % n ;
   edges[jj].second = (edges[jj].second + n - jp) % n ;
   swap(edges[ii].second, edges[jj].second) ;
unwindit2:
   for (int i=0; i<n; i++)
      bv[i] = sbv[i] ;
   return 0 ;
}
const long long maxtrial = 16000000 ;
void swaptwo() {
   int ec = edges.size() ;
   for (long long trial=1; ; trial++) {
      if (trial == maxtrial) {
         cout << "Trial " << trial << " ec " << ec << endl ;
         for (int i=0; i<n; i++) {
            cout << i ;
            for (int j=0; j<n; j++)
               if ((bv[i] >> j) & 1)
                  cout << " " << j ;
            cout << endl ;
         }
         exit(0) ;
      }
      int i = (int)(ec*drand48()) ;
      swap(edges[i].first, edges[i].second) ;
      int j = (int)(ec*drand48()) ;
      while (i == j)
         j = (int)(ec*drand48()) ;
      if (edges[i].first == edges[j].first
#ifdef QUICK
          ||
          edges[i].first == edges[j].second ||
          edges[i].second == edges[j].first ||
          edges[i].second == edges[j].second ||
          ((bv[edges[i].first] >> edges[j].second) & 1) ||
          ((bv[edges[i].second] >> edges[j].first) & 1)
#endif
          ) {
         continue ;
      }
      int ip = mod * ((int)(ddiv*drand48())) ;
      int jp = mod * ((int)(ddiv*drand48())) ;
      if (!doswap(i, j, ip, jp))
         continue ;
      return ;
   }
}
double multiplier = 0.99 ;
double maxtime = 1e9 ;
int main(int argc, char *argv[]) {
   srand48(time(0)) ;
#ifdef BIG
   ull one = 1 ;
   onebit[0] = one ;
   for (int i=1; i<128; i++)
      onebit[i] = onebit[i-1] + onebit[i-1] ;
#endif
   while (argc > 1 && argv[1][0] == '-') {
      argc-- ;
      argv++ ;
      switch (argv[0][1]) {
case 'm':
         multiplier = atof(argv[1]) ;
         argc-- ;
         argv++ ;
         break ;
case 'T':
         maxtime = atof(argv[1]) ;
         argc-- ;
         argv++ ;
         break ;
default:
         cout << "Bad argument " << argv[0] << endl ;
         exit(0) ;
      }
   }
   start = walltime() ;
   pstart = walltime() ;
   bool ingraph = 0 ;
   bool early = 0 ;
   n = atol(argv[1]) ;
   k = atol(argv[2]) ;
   ddiv = atol(argv[3]) ;
   if (n % ddiv != 0) {
      cout << "Bad args" << endl ;
      exit(0) ;
   }
   mod = n / ddiv ;
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
   for (int ii=0; left; ii++) {
      int i = (int)((n-1)*drand48()) ;
      if ((bv[0] >> ((i+1)%n)) & 1)
         continue ;
      if (i+1+i+1 == n)
         continue ;
      if (left >= n) {
         for (int j=0; j<n; j++)
            addedge(j, (j+i+1)%n) ;
         left -= n ;
      } else {
         if (i % 2 == 1)
            continue ;
         if ((bv[0] >> ((i+1)%n)) & 1)
            continue ;
         for (int j=0; j<n; j += 2)
            addedge(j, (j+i+1)%n) ;
         left -= n/2 ;
      }
   }
   for (int i=0; i<n; i++)
      if (bc(bv[i]) != k) {
 cerr << "At " << i << " bc is " << bc(bv[i]) << endl ;
         error("! bad state right away") ;
      }
   if ((int)edges.size() != n * k / (2 * ddiv)) {
      cout << "! bad size for this mod" << endl ;
      exit(0) ;
   }
 cout << "Edges is " << edges.size() << endl ;
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
         if (!doswap(lasti, lastj, n-lastjp, n-lastip))
            error("! failed in undo") ;
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
         if (walltime() - pstart > maxtime)
            break ;
      }
      if ((processed & (processed - 1)) == 0)
         cout << "Processed " << processed << " in " << walltime()-start << " frac " << frac << endl << flush ;
   }
}
