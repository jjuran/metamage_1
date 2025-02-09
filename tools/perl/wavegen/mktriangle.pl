#!/usr/bin/env perl

use warnings;
use strict;

my $ascent = join "", map { chr ($_ * 2 + 1) } 0 .. 127;

my $opening = substr( $ascent, 64, 64 );
my $closing = substr( $ascent,  0, 64 );
my $descent = reverse $ascent;

print join "", $opening, $descent, $closing;
