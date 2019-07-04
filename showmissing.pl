open F, "scanall.log" or die "Can't read scanall.log" ;
while (<F>) {
   $d = '?' ;
   / excess (\d+) / ;
   $e = $1 ;
   / dia (\d+)/ ;
   $d = $1 ;
   @f = split " ", $_ ;
   $n = $f[1] ;
   $k = $f[3] ;
   $d[$n][$k] = $d ;
   $e[$n][$k] = $e ;
   next if / ratio 1 / ;
   next if / excess 0 / ;
   $v[$n] |= 1<<$k ;
}
for ($i=0; $i<@v; $i++) {
   next if !$v[$i] ;
   printf("%2d:", $i) ;
   for ($j=3; $j<=10; $j++) {
      if (($v[$i] >> $j) & 1) {
         $s = sprintf("%d:%d,%d", $j, $e[$i][$j], $d[$i][$j]) ;
         printf(" %-7s", $s) ;
         $v[$i] -= 1<<$j ;
         last if $v[$i] == 0 && $d[$i][$j] <= 2 ;
      } elsif (defined($d[$i][$j])) {
         printf("   %-5d", $d[$i][$j]) ;
         last if $v[$i] == 0 && $d[$i][$j] <= 2 ;
      } else {
         print "        " ;
      }
   }
   print "\n" ;
}
