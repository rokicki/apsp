for ($n=4; $n<=64; $n++) {
   for ($k=2; $k<$n; $k++) {
      next if ($n & 1) && ($k & 1) ;
      system("perl scan.pl $n $k") ;
   }
}
