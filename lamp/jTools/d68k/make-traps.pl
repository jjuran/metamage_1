#!/usr/bin/perl

use warnings;
use strict;

sub silly
{
	/DogCow/ ||
	/Moof/   ||
	/Pinafore/;
}

sub get_traps_file
{
	my $intfs_libs_path = "$ENV{HOME}/Developer/Applications/Interfaces&Libraries/";
	my $cincludes_path  = "$intfs_libs_path/Interfaces/CIncludes/";
	my $traps_h         = "$cincludes_path/Traps.h";
	
	return $traps_h;
}

my $traps_h = shift || get_traps_file();

-f $traps_h or die "Nonexistent $traps_h\n";

open my $in, "<", $traps_h or die "Can't open Traps.h.\n";

my $blob = <$in>;

undef $in;

my @lines = grep { /^  _\w+\s+= 0x..../ && !silly() } split "\r", $blob;

undef $blob;

foreach my $line ( @lines )
{
	my ( $name, $word ) = $line =~ m/  (_\w+)\s+= 0x(....)/;
	
	$line = "$word\t$name\n";
}

print join "", @lines;

