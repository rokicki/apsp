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
   $v[$n] |= 1<<$k ;
}
for $f (glob "apsp-*.log") {
   $f =~ /apsp-(\d+)-(\d+)/ or die "No match in $f" ;
   $n = $1 ;
   $k = $2 ;
   push @{$f[$n][$k]}, $f ;
}
for $f (glob "oldlogs/apsp-*.log") {
   $f =~ /apsp-(\d+)-(\d+)/ or die "No match in $f" ;
   $n = $1 ;
   $k = $2 ;
   push @{$f[$n][$k]}, $f ;
}
for ($n=4; $n<=128; $n++) {
   print " [$n]" ;
   flush STDOUT ;
   for ($k=3; $k+1<$n; $k++) {
      next if ($k & 1) && ($n & 1) ;
      if (@{$f[$n][$k]}) {
         $e = $e[$n][$k] ;
         if (-f "GRAPH-$n-$k-$e.g6") {
            system("~/nauty/nauty27rc2/showg -o1 < GRAPH-$n-$k-$e.g6 | ./apsp > t.adj") ;
            unshift @{$f[$n][$k]}, "t.adj" ;
         }
         system("cat @{$f[$n][$k]} | ./gutil -n $n -k $k -x $e | ~/nauty/nauty27rc2/shortg -q > GRAPH-$n-$k-$e.g6") ;
      }
   }
}
