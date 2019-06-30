open F, "scanall.log" or die "Can't read scanall.log" ;
while (<F>) {
   $d = '?' ;
   / excess (\d+) / ;
   $e = $1 ;
   / dia (\d+) / ;
   $d = $1 ;
   @f = split " ", $_ ;
   $n = $f[1] ;
   $k = $f[3] ;
   $d[$n][$k] = $d ;
   $e[$n][$k] = $e ;
}
for ($n=4; $n<=64; $n++) {
   for ($k=3; $k+1<$n; $k++) {
      next if ($k & 1) && ($n & 1) ;
      $e = $e[$n][$k] ;
      if (-f "GRAPH-$n-$k-$e.g6") {
         system("head -n 1 < GRAPH-$n-$k-$e.g6 | ./toedgelist > SUBMIT-$n-$k-$e.g6") ;
      }
   }
}
