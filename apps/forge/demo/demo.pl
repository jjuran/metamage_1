#!/usr/bin/perl

use warnings;
use strict;

use FindBin '$RealBin';

$ENV{ DEMO } = $RealBin;

chdir( "/gui/new/port" ) or die "$!";

sub setup
{
	system "$RealBin/setup";
}

sub finish
{
	system "/usr/bin/daemonize --cwd --ctty=tty -- /usr/bin/idle";
}

sub go
{
	my ( $name ) = @_;
	
	my $path = "$RealBin/$name";
	
	system $path;
}

setup();

go( "main" );

finish();

