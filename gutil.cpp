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
typedef unsigned long long ull ;
const int MAXBUF = 1000 ;
char mbuf[MAXBUF+1] ;
int e ;
int n ;
const int MAXN = 64 ;
const int INF = MAXN ;
ull bv[MAXN] ;
/* assumes bv is valid */
int diameter ;
void error(const char *s) {
   cerr << s << endl ;
   exit(10) ;
}
int fastapsp() {
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
         return 1000000000 ;
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
   bv[a] |= 1LL << b ;
   bv[b] |= 1LL << a ;
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
      error("! saw a smaller sum") ;
   }
   return 0 ;
}
int main(int argc, char *argv[]) {
   while (argc > 1 && argv[1][0] == '-') {
      argc-- ;
      argv++ ;
      switch (argv[0][1]) {
case 'x':
         excess = atol(argv[1]) ;
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
         if (n) {
            processgraph() ;
            newgraph() ;
         }
      }
   }
   if (n)
      processgraph() ;
}
