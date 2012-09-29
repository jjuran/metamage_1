#!/usr/bin/perl

use warnings;
use strict;

use FindBin '$RealBin';

$ENV{ DEMO } = $RealBin;

exec "$RealBin/demo";

