/*
 *   Read graphs; filter them; output them in g6 format.
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
typedef __uint128_t ull ;
const int MAXBUF = 1000 ;
char mbuf[MAXBUF+1] ;
int e ;
int n ;
const int MAXN = 128 ;
const int INF = MAXN ;
ull bv[MAXN] ;
ull onebit[128] ;
/* assumes bv is valid */
int diameter ;
void error(const char *s) {
   cerr << s << endl ;
   exit(10) ;
}
int bigpopcount(ull v) {
   unsigned long long t1 = v ;
   unsigned long long t2 = (v >> 64) ;
   return __builtin_popcountll(t1) + __builtin_popcountll(t2) ;
}
int bigffs(ull v) {
   unsigned long long t1 = v ;
   if (t1)
      return ffsll(t1) ;
   return 64 + ffsll(v >> 64) ;
}
int fastapsp() {
   ull seen[MAXN] ;
   ull dodd[MAXN] ;
   ull deven[MAXN] ;
   for (int i=0; i<n; i++)
      seen[i] = onebit[i] | bv[i] ;
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
            m ^= onebit[b] ;
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
            m ^= onebit[b] ;
            r |= deven[b] ;
         }
         dodd[i] = r ;
         ull nw = r & ~seen[i] ;
         seen[i] |= r ;
         left -= bigpopcount(nw) ;
      }
      if (left == oleft)
         return 100000000 ;
      if (left == 0)
         break ;
      sum += left ;
      d++ ;
   }
   diameter = d ;
   return sum >> 1 ;
}
int k ;
vector<pair<int, int> > edges ;
void newgraph() {
   edges.clear() ;
   for (int i=0; i<MAXN; i++)
      bv[i] = 0 ;
   n = 0 ;
}
void addedge(int a, int b) {
   if ((bv[a] >> b) & 1)
      return ;
   bv[a] |= onebit[b] ;
   bv[b] |= onebit[a] ;
   edges.push_back({a,b}) ;
   if (a >= n)
      n = a + 1 ;
   if (b >= n)
      n = b + 1 ;
}
int excess = -1 ;
void emitn(int n) {
   if (n <= 62)
      putchar(63+n) ;
   else {
      putchar(126) ;
      putchar(63+(n>>12)) ;
      putchar(63+(63&(n>>6))) ;
      putchar(63+(63&n)) ;
   }
}
int bitaccum = 0 ;
int nbits = 0 ;
void clearbit() {
   nbits = 0 ;
   bitaccum = 0 ;
}
void flushbit() {
   if (nbits > 0) {
      putchar(63+bitaccum) ;
      clearbit() ;
   }
}
void emitbit(int b) {
   bitaccum |= b << (5-nbits) ;
   nbits++ ;
   if (nbits == 6)
      flushbit() ;
}
void emitg() {
   emitn(n) ;
   clearbit() ;
   for (int i=0; i<n; i++)
      for (int j=0; j<i; j++)
         emitbit((bv[i] >> j) & 1) ;
   flushbit() ;
   putchar(10) ;
}
int processgraph() {
   e = (int)edges.size() ;
   if (e == 0)
      return 0 ;
   if ((2 * e) % n != 0) {
      cerr << "E is " << e << " n " << n << endl ;
      return 0 ;
   }
   k = 2 * e / n ;
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
   sum = sum * n / 2 ;
   int check = fastapsp() ;
   int hi = diameter ;
   if (excess == -1 || check == sum + excess) {
      emitg() ;
   } else if (check < sum + excess) {
      cerr << "check " << (check-sum) << " claimed " << excess << " n " << n << " k " << k << endl ;
      error("! saw a smaller sum") ;
   }
   return 0 ;
}
int kforce = -1 ;
int nforce = -1 ;
int check(int n) {
   if (n == 0)
      return 0 ;
   if (nforce > 0 && n != nforce)
      return 0 ;
   if (kforce > 0 && (int)edges.size() != n * kforce / 2)
      return 0 ;
   return 1 ;
}
int main(int argc, char *argv[]) {
   ull one = 1 ;
   onebit[0] = one ;
   for (int i=1; i<128; i++)
      onebit[i] = onebit[i-1] + onebit[i-1] ;
   while (argc > 1 && argv[1][0] == '-') {
      argc-- ;
      argv++ ;
      switch (argv[0][1]) {
case 'x':
         excess = atol(argv[1]) ;
         argc-- ;
         argv++ ;
         break ;
case 'n':
         nforce = atol(argv[1]) ;
         argc-- ;
         argv++ ;
         break ;
case 'k':
         kforce = atol(argv[1]) ;
         argc-- ;
         argv++ ;
         break ;
default:
         cout << "Bad argument " << argv[0] << endl ;
         exit(0) ;
      }
   }
   newgraph() ;
   while (fgets(mbuf, MAXBUF, stdin)) {
      char *p = mbuf ;
      while (*p && *p <= ' ')
         p++ ;
      if ('0' <= *p && *p <= '9') {
         int a = strtol(p, &p, 10) ;
         while (*p && (*p < '0' || *p > '9'))
            p++ ;
         while (1) {
            char *oldp = p ;
            int b = strtol(p, &p, 10) ;
            if (p == oldp)
               break ;
            addedge(a, b) ;
         }
      } else {
         if (check(n)) {
            processgraph() ;
         }
         newgraph() ;
      }
   }
   if (check(n))
      processgraph() ;
}
