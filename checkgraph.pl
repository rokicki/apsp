for $f (glob "GRAPH-*") {
   $f =~ /GRAPH-(\d+)-(\d+)-(\d+).g6/ or die "Can't parse file name $f" ;
   $n = $1 ;
   $k = $2 ;
   $x = $3 ;
   $good = 0 ;
   $bad = 0 ;
   @good = () ;
   open F, "./showg6 < $f |" or die "Can't fork" ;
   while (<F>) {
      @f = split " ", $_ ;
      if (($f[1] != $n) || ($f[3] != $k) || $f[7] != $x) {
         print "Mismatch $f [@f]\n" ;
         $bad++ ;
      } else {
         push @good, $_ ;
         $good++ ;
      }
   }
   close F ;
   if ($good == 0 || $bad != 0) {
      print "$f bad $bad good $good; rewriting\n" ;
      open F, ">$f" or die "Can't rewrite $f\n" ;
      for (@good) {
         print F $_ ;
      }
      close F ;
   }
}
