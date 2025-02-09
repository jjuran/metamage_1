#!/usr/bin/env perl

use warnings;
use strict;

my $opening = (chr 192) x 128;
my $closing = (chr  64) x 128;

print join "", $opening, $closing;
