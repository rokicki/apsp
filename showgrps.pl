for ($n=3; $n<=64; $n++) {
   for ($k=3; $k+1<$n; $k++) {
      next if ($k & 1) && ($n & 1) ;
      @f = glob "GRAPH-$n-$k-*" ;
      for $f (@f) {
         $f =~ /GRAPH-\d+-\d+-(\d+)/ ;
         $d = $1 ;
         system("./autom < @f | ~/nauty/nauty27rc2/dreadnaut  | grep grpsize > AUTOM-$n-$k-$d.log") ;
      }
   }
}
