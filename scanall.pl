my $only = shift ;
open F, "scanall.log" or die "Can't read scanall.log" ;
while (<F>) {
   chomp ;
   @f = split " ", $_ ;
   $n = $f[1] ;
   $k = $f[3] ;
   $e = $f[9] ;
   $e[$n][$k] = $e ;
   $lin[$n][$k] = $_ ;
}
close F ;
for ($n=4; $n<=128; $n++) {
   for ($k=2; $k<$n; $k++) {
      next if ($n & 1) && ($k & 1) ;
      next if !defined($e[$n][$k]) ;
      if ((defined($only) && $only != $n) || $e[$n][$k] == 0) {
         print "$lin[$n][$k]\n" ;
      } else {
         system("perl scan.pl $n $k") ;
      }
   }
}
