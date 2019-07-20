open F, "rokicki.0701.csv" or die "Can't read rokicki.0701.csv" ;
while (<F>) {
   chomp ;
   @f = split ",", $_ ;
   $md5 = $f[-1] ;
   $submitted{$md5}++ ;
}
close F ;
open F, "submit-md5" or die "Can't read submit-md5" ;
while (<F>) {
   chomp ;
   @f = split " ", $_ ;
   $md5 = $f[-1] ;
   $filename = $f[1] ;
   $filename =~ s/[()]//g ;
   $filename =~ /SUBMIT-(\d+)-(\d+)-(\d+)/ or die "Can't parse filename" ;
   $n = $1 ;
   $k = $2 ;
   $x = $3 ;
   if ($submitted{$md5}) {
      $best[$n][$k] = $x if !defined($best[$n][$k]) || $x < $best[$n][$k] ;
   }
}
close F ;
open F, "find submitted -name '*.g6' |" or die "Can't fork" ;
while (<F>) {
   chomp ;
   /SUBMIT-(\d+)-(\d+)-(\d+)/ or die "Can't parse" ;
   $n = $1 ;
   $k = $2 ;
   $x = $3 ;
   $best[$n][$k] = $x if !defined($best[$n][$k]) || $x < $best[$n][$k] ;
}
close F ;
open F, "scanall.log" or die "Can't read scanall" ;
while (<F>) {
   chomp ;
   @f = split " ", $_ ;
   $n = $f[1] ;
   $k = $f[3] ;
   $x = $f[9] ;
   next if $k == 2 ;
   next if $k + 1 == $n ;
   $d = $f[11] ;
   if ($d == 2) {
      $low2[$n] = $k if !defined($low2[$n]) ;
      next if $k > $low2[$n] ;
   }
   if (!defined($best[$n][$k]) || $best[$n][$k] != $x) {
      next if -f "tosubmit/SUBMIT-$n-$k-$x.g6" ;
      print " [$n,$k]" ;
      flush STDOUT ;
      if (-f "GRAPH-$n-$k-$x.g6") {
         system("head -n 1 < GRAPH-$n-$k-$x.g6 | ./toedgelist > tosubmit/SUBMIT-$n-$k-$x.g6") ;
      } else {
         die "Can't find GRAPH-$n-$k-$x.g6" ;
      }
   }
}
close F ;
