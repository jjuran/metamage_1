#!/usr/bin/env perl

use warnings;
use strict;

my $ascent = join "", map { chr $_ } 0 .. 255;

my $opening = substr( $ascent, 128, 128 );
my $closing = substr( $ascent,   0, 128 );

print join "", $opening, $closing;
