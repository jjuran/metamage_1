#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

use FindBin '$RealBin';

$ENV{ DEMO } = $RealBin;

exec "$RealBin/demo";

