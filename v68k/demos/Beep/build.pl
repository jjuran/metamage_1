#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

use FindBin '$RealBin';

sub run
{
	system( @_ ) == 0 or exit 1;
}

my $home = $ENV{ HOME };

my $out = "$home/var/build/misc/Beep";

run( qw( mkdir -p ), $out );

chdir $out or die "chdir: $out: $!\n";

unlink "beep.c";

run( "ln", "-a", "$RealBin/beep.c", "beep.c" );

run( qw( tlsrvr -- SC beep.c -o beep.o ) );
run( qw( tlsrvr -- Link -m main beep.o -o Beep ) );

run( "open", $out );
