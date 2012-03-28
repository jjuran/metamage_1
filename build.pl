#!/usr/bin/perl

use FindBin '$RealBin';

use lib "$RealBin/tools/perl";

use Compile::Driver;

use warnings;
use strict;


Compile::Driver::main( @ARGV );

