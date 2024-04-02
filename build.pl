#!/usr/bin/env perl

use FindBin '$RealBin';

use lib "$RealBin/tools/perl";

use Compile::Driver;

use warnings FATAL => 'all';
use strict;


$ENV{PATH}="$ENV{PATH}:$RealBin/var/out";

Compile::Driver::main( @ARGV );
