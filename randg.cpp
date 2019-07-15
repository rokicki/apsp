#include <iostream>
#include <cstdlib>
#include <sys/time.h>
using namespace std ;
typedef long long ll ;
const int MAXN = 128 ;
int deg[MAXN] ;
int adj[MAXN][MAXN] ;
ll cnt = 1 ;
int main(int argc, char *argv[]) {
   srand48(time(0)) ;
   int n = atol(argv[1]) ;
   int k = atol(argv[2]) ;
   if (argc > 3)
      cnt = atoll(argv[3]) ;
   if ((n & 1) && (k & 1)) {
      cout << "Bad args" << endl ;
      exit(0) ;
   }
   while (cnt > 0) {
      while (1) {
         for (int i=0; i<n; i++) {
            deg[i] = 0 ;
            for (int j=0; j<n; j++)
               adj[i][j] = 0 ;
         }
         int needed = n * k / 2 ;
         ll countdown = 4 * n * n ;
         while (needed) {
            if (countdown-- < 0)
               break ;
            int a = (int)(drand48()*n) ;
            int b = (int)(drand48()*n) ;
            if (a == b || deg[a] == k || deg[b] == k || adj[a][b])
               continue ;
            countdown = 4 * n * n ;
            adj[a][b] = adj[b][a] = 1 ;
            deg[a]++ ;
            deg[b]++ ;
            needed-- ;
         }
         if (needed == 0)
            break ;
      }
      cout << "G" << endl ;
      for (int i=0; i<n; i++) {
         cout << (i+1) << " :" ;
         for (int j=0; j<n; j++)
            if (adj[i][j] == 1)
               cout << " " << (j+1) ;
         cout << endl ;
      }
      cout << "T" << endl << flush ;
      cnt-- ;
   }
}
