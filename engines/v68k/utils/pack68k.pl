#!/usr/bin/perl

use warnings;
use strict;

while (<>)
{
	next if m{^\s*(\#.*)?$};  # skip blanks and comments
	
	s{\s}{}g;  # remove whitespace
	
	s{ ([0-9A-Fa-f]{2}) }{chr hex $1}gxe;
	
	print;
}

