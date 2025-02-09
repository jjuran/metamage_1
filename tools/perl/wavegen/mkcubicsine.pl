#!/usr/bin/env perl

use warnings;
use strict;

use Math::Trig;

sub cubed
{
	my ($x) = @_;
	
	return $x ** 3;
}

sub u8
{
	my ( $normal ) = @_;
	
	# $normal range:  -1   ..   1
	# Add 1:           0   ..   2
	# Mul by 127:      0   .. 254
	# Add 1.5:         1.5 .. 255.5
	# Round down:      1   .. 255
	
	return int (($normal + 1) * 127 + 1.5)
}

print join "", map { chr u8 cubed sin ($_ * 2 * pi / 256) } 0 .. 255;
