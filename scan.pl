my $n = shift ;
my $k = shift ;
#
#   Find the best value.
#
die "Bad values" if $k < 2 || $k >= $n || (($n & 1) && ($k & 1)) ;
my $sum = 0 ;
my $d = 1 ;
my $c = $k ;
my $left = $n-1 ;
while ($left > 0) {
   $t = $c ;
   $t = $left if $t > $left ;
   $sum += $d * $t ;
   $d++ ;
   $left -= $t ;
   $c = $c * ($k - 1) ;
}
$opt = $sum / ($n - 1) ;
my $gavg = $n ;
$best = 1000000 ;
for $f (glob "GRAPH-$n-$k-*.g6") {
   $f =~ /GRAPH-\d+-\d+-(\d+).g6/ or die "Bad file" ;
   $best = $1 if $1 < $best ;
}
if ($best < 1e6) {
   $gavg = $opt + 2 * $best / ($n * ($n - 1)) ;
}
@f1 = glob "apsp-$n-$k*.log" ;
@f2 = glob "oldlogs/apsp-$n-$k*.log" ;
for $f (@f1, @f2) {
   $f =~ /apsp-\d+-(\d+)/ ;
   next if $1 != $k ;
   open F, $f or die "Can't read $f" ;
   my $bestavg = undef ;
   my $dia = undef ;
   my $finished = 0 ;
   while (<F>) {
      chomp ;
      if (/Order \S+ diameter (\S+) avg (\S+)/) {
         if (!defined($bestavg) || ($bestavg > $2)) {
            $dia = $1 ;
         }
         $bestavg = $2 if !defined($bestavg) || $2 < $bestavg ;
      } elsif (/Order \S+ degree \d+ diameter (\S+) avg (\S+)/) {
         if (!defined($bestavg) || ($bestavg > $2)) {
            $dia = $1 ;
         }
         $bestavg = $2 if !defined($bestavg) ||  $2 < $bestavg ;
      } elsif (/Processed/) {
         $finished = 1 ;
      }
   }
   close F ;
   if (defined($bestavg) && $bestavg < $gavg) {
      $gavg = $bestavg ;
      $gdia = $dia ;
   }
}
exit(0) if $gavg == $n ;
$r = $gavg / $opt ;
$excess = int(($gavg-$opt) * $n * ($n-1) / 2 + 0.5) ;
if (abs($gavg-$opt) < 1/($n*$n)) {
   $r = 1 ;
   $excess = 0 ;
}
if (!defined($gdia)) {
   open F, "head -n 1 GRAPH-$n-$k-$best.g6 | ./apspg6 $k |" or die "Can't parse" ;
   while (<F>) {
      if (/diameter (\S+)/) {
         $gdia = $1 ;
      }
   }
   close F ;
}
print "N $n k $k opt $opt avg $gavg excess $excess dia $gdia\n" ;
