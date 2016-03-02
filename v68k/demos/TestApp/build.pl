#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

use FindBin '$RealBin';

my $NAME = "Test";

my $base = lc $NAME;

my $dir = $NAME;
my $app = $NAME;
my $src = "$base.c";
my $obj = "$base.o";

sub run
{
	system( @_ ) == 0 or exit 1;
}

my $home = $ENV{ HOME };

my $out = "$home/var/build/misc/$dir";

run( qw( mkdir -p ), $out );

chdir $out or die "chdir: $out: $!\n";

unlink $src;  # alias to source file

run( "ln", "-a", "$RealBin/$src", "$src" );

run( split / /, "tlsrvr -- SC $src -o $obj" );
run( split / /, "tlsrvr -- Link -m main $obj -o $app" );

run( "open", $out );
