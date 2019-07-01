while (<>) {
   if (/ : /) {
      chomp ;
      @f = split " ", $_ ;
      splice(@f, 0, 2) ;
      @f = map { $_ - 1 } @f ;
      @seen = () ;
      $seen[$_]++ for @f ;
      @f = () ;
      for ($i=0; $i<8; $i++) {
         push @f, ($seen[$i] ? 1 : 0) ;
      }
      print join(", ", @f) ;
      print "\n" ;
   } else {
      print ;
   }
}
