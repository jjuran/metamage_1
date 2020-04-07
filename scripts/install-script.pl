#!/usr/bin/perl

use warnings;
use strict;

my $USAGE = "usage: INTERPRETER=<interpreter-path> $0 <src> <dst>\n";

my $interpreter = $ENV{INTERPRETER} or die $USAGE;

my $src = shift or die $USAGE;
my $dst = shift or die $USAGE;

my $script = do { local(@ARGV, $/) = $src; <> };

$script =~ s{^#!/usr/bin/env \S+}{#!$interpreter};

open my $out, ">", $dst or die "$dst: $!\n";
print $out $script      or die "$dst: $!\n";

my @stat = stat $out    or die "$dst: $!\n";

my $perm = $stat[2] & 07777 | 0111;

chmod $perm, $out       or die "$dst: $!\n";
close $out              or die "$dst: $!\n";

exit 0;
