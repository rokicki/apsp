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
typedef unsigned long long bull ;
const int MBITSETSIZE = 8 ;
const int BITSPERLONG = 64 ;
const int LOWMASK = 63 ;
const int SHIFT = 6 ;
template<size_t N> class mbitset {
   bull v[N] ;
public:
   mbitset<N>() {
      for (size_t i=0; i<N; i++) v[i] = 0 ;
   }
   bool operator[](size_t pos) const { return (v[pos>>SHIFT]>>(pos&LOWMASK)) & 1 ; }
   void setbit(size_t pos) { v[pos>>SHIFT]|=1LL<<(pos&LOWMASK) ; }
   void clearbit(size_t pos) { v[pos>>SHIFT]&=~(1LL<<(pos&LOWMASK)) ; }
   int count() const {
      int r = 0 ;
      for (size_t i=0; i<N; i++)
         r += __builtin_popcountll(v[i]) ;
      return r ;
   }
   mbitset<N>& operator|=(const mbitset<N>&b) {
      for (size_t i=0; i<N; i++)
         v[i]|=b.v[i] ;
      return *this ;
   }
   mbitset<N>& operator&=(const mbitset<N>&b) {
      for (size_t i=0; i<N; i++)
         v[i]|=b.v[i] ;
      return *this ;
   }
   void negit(const mbitset<N>&b) {
      for (size_t i=0; i<N; i++)
         v[i]&=~b.v[i] ;
   }
   void orleft(const mbitset<N>&b, int sh) {
      int bsh = sh & LOWMASK ;
      size_t wsh = sh >> SHIFT ;
      if (bsh == 0) {
         for (size_t i=wsh; i<N; i++)
            v[i] |= b.v[i-wsh] ;
      } else {
         int nbsh = BITSPERLONG - bsh ;
         v[wsh] |= (b.v[0] << bsh) ;
         for (size_t i=wsh+1; i<N; i++)
            v[i] |= (b.v[i-wsh] << bsh) | (b.v[i-wsh-1] >> nbsh) ;
      }
   }
   void orright(const mbitset<N>&b, int sh) {
      int bsh = sh & LOWMASK ;
      size_t wsh = sh >> SHIFT ;
      if (bsh == 0) {
         for (size_t i=wsh; i<N; i++)
            v[i-wsh] |= b.v[i] ;
      } else {
         int nbsh = BITSPERLONG - bsh ;
         v[N-1-wsh] |= b.v[N-1] >> bsh ;
         for (size_t i=N-1; i>wsh; i--)
            v[i-1-wsh] |= (b.v[i-1] >> bsh) | (b.v[i] << nbsh) ;
      }
   }
   void clean() {
      for (size_t i=0; i<N; i++)
         v[i] &= gmask.v[i] ;
   }
   void show() {
      for (int i=0; i<n; i++)
         cout << (*this)[i] ;
      cout << endl ;
   }
   static void setn(int n_) {
      n = n_ ;
      for (int i=0; i<n; i++)
         gmask.setbit(i) ;
   }
   static mbitset gmask ;
   static int n ;
} ;
template<size_t N> mbitset<N> mbitset<N>::gmask ;
template<size_t N> int mbitset<N>::n ;
typedef mbitset<MBITSETSIZE> ull ;
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
const int MAXN = MBITSETSIZE * BITSPERLONG ;
const int INF = MAXN ;
int nmod[2*MAXN] ;
int mmod[2*MAXN] ;
ull bv[MAXN] ;
ull sbv[MAXN] ;
/* assumes bv is valid */
int diameter ;
vector<pair<int, int> > edges ;
int edgecnts[MAXN] ;
int edgeloc[MAXN][MAXN] ;
ull seen[MAXN] ;
ull dodd[MAXN] ;
ull deven[MAXN] ;
double fastapsp() {
   for (int i=0; i<mod; i++)
      edgecnts[i] = 0 ;
   for (int i=0; i<(int)edges.size(); i++) {
      int a = edges[i].first ;
      int b = edges[i].second ;
      int a1 = mmod[a] ;
      int b1 = nmod[b+n+a1-a] ;
      edgeloc[a1][edgecnts[a1]++] = b1 ;
      b1 = mmod[b] ;
      a1 = nmod[a+n+b1-b] ;
      edgeloc[b1][edgecnts[b1]++] = a1 ;
   }
   for (int i=0; i<mod; i++) {
      seen[i] = bv[i] ;
      seen[i].setbit(i) ;
   }
   int d = 1 ;
   int left = n * mod - mod - 2 * e / ddiv ;
   int sum = 2 * e / ddiv + left * 2 ;
   for (int i=0; i<mod; i++)
      dodd[i] = bv[i] ;
   if (left)
      d++ ;
   while (left) {
      int oleft = left ;
      for (int i=0; i<mod; i++) {
         ull r ;
         for (int jj=0; jj<edgecnts[i]; jj++) {
            int b = edgeloc[i][jj] ;
            int blo = mmod[b] ;
            int sh = b - blo ;
            if (sh == 0)
               r |= dodd[blo] ;
            else {
               r.orleft(dodd[blo], sh) ;
               r.orright(dodd[blo], n-sh) ;
            }
         }
         r.clean() ;
         deven[i] = r ;
         r.negit(seen[i]) ;
         seen[i] |= r ;
         left -= r.count() ;
      }
      if (left == 0)
         break ;
      d++ ;
      sum += left ;
      for (int i=0; i<mod; i++) {
         ull r ;
         for (int jj=0; jj<edgecnts[i]; jj++) {
            int b = edgeloc[i][jj] ;
            int blo = mmod[b] ;
            int sh = b - blo ;
            if (sh == 0)
               r |= deven[blo] ;
            else {
               r.orleft(deven[blo],sh) ;
               r.orright(deven[blo],n-sh) ;
            }
         }
         r.clean() ;
         dodd[i] = r ;
         r.negit(seen[i]) ;
         seen[i] |= r ;
         left -= r.count() ;
      }
      if (left == oleft)
         return 1e20 ;
      if (left == 0)
         break ;
      sum += left ;
      d++ ;
   }
   diameter = d ;
   return sum / (double)(mod*(n-1)) ;
}
double goal = 0 ;
int k ;
void addedge(int aa, int bb) {
   if ((bv[aa][bb]))
      return ;
   for (int p=0; p<n; p += mod) {
      int a = (aa + p) % n ;
      int b = (bb + p) % n ;
      bv[a].setbit(b) ;
      bv[b].setbit(a) ;
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
            if ((bv[i][j]))
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
      int i = nmod[(edges[ii].first + p)] ;
      int j = nmod[(edges[ii].second + p)] ;
      if (0 == ((bv[i][j]))) {
// cout << "Fail 1" << endl ;
         goto unwindit2 ;
      }
      bv[i].clearbit(j) ;
      bv[j].clearbit(i) ;
      i = nmod[(edges[jj].first + p)] ;
      j = nmod[(edges[jj].second + p)] ;
      if (0 == ((bv[i][j]))) {
// cout << "Fail 2" << endl ;
         goto unwindit2 ;
      }
      bv[i].clearbit(j) ;
      bv[j].clearbit(i) ;
   }
   swap(edges[ii].second, edges[jj].second) ;
   edges[ii].second = nmod[(edges[ii].second + ip)] ;
   edges[jj].second = nmod[(edges[jj].second + jp)] ;
   for (int p=0; p<n; p+=mod) {
      int i = nmod[(edges[ii].first + p)] ;
      int j = nmod[(edges[ii].second + p)] ;
      if (i == j || 1 == ((bv[i][j]))) {
// cout << "Fail 3 " << " " << i << " " << j << " " << ip << " " << jp << endl ;
         goto unwindit ;
      }
      bv[i].setbit(j) ;
      bv[j].setbit(i) ;
      i = nmod[(edges[jj].first + p)] ;
      j = nmod[(edges[jj].second + p)] ;
      if (i == j || 1 == ((bv[i][j]))) {
// cout << "Fail 4 " << " " << i << " " << j << " " << ip << " " << jp << endl ;
         goto unwindit ;
      }
      bv[i].setbit(j) ;
      bv[j].setbit(i) ;
   }
   lasti = ii ;
   lastj = jj ;
   lastip = ip ;
   lastjp = jp ;
// for (int i=0; i<n; i++)
//    if (bc(bv[i]) != k) {
//       error("! bad state") ;
//    }
   return 1 ;
unwindit:
   edges[ii].second = nmod[(edges[ii].second + n - ip)] ;
   edges[jj].second = nmod[(edges[jj].second + n - jp)] ;
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
               if (bv[i][j])
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
          ((bv[edges[i].first][edges[j].second)]) ||
          ((bv[edges[i].second][edges[j].first)])
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
/*
 *   Test the bitset
 *
   for (int i=0; i<10000; i++) {
      ull t1, t2 ;
      for (int j=0; j<10; j++) {
         int bb = (int)(512*drand48()) ;
         t1.setbit(bb) ;
      }
      int sh = (int)(512*drand48()) ;
      if (drand48() < 0.5) {
         t2.orright(t1, sh) ;
         sh = - sh ;
      } else {
         t2.orleft(t1, sh) ;
      }
      for (int j=0; j<512; j++) {
         int j2 = j + sh ;
         if (j2 < 0 || j2 >= 512)
            continue ;
         if (t1[j] != t2[j2])
            cout << "Fail with shift " << sh << endl ;
      }
   }
   exit(0) ;
 */
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
   mbitset<MBITSETSIZE>::setn(n) ;
   k = atol(argv[2]) ;
   ddiv = atol(argv[3]) ;
   for (int i=0; i<2*MAXN; i++)
      nmod[i] = i % n ;
   if (n % ddiv != 0) {
      cout << "Bad args" << endl ;
      exit(0) ;
   }
   mod = n / ddiv ;
   for (int i=0; i<2*MAXN; i++)
      mmod[i] = i % mod ;
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
      if ((bv[0][(i+1)%n]))
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
         if ((bv[0][(i+1)%n]))
            continue ;
         for (int j=0; j<n; j += 2)
            addedge(j, (j+i+1)%n) ;
         left -= n/2 ;
      }
   }
   for (int i=0; i<n; i++)
      if (bv[i].count() != k) {
 cerr << "At " << i << " bc is " << bv[i].count() << endl ;
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
