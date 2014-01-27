#!/usr/bin/env jtest

$ readlink /proc/$$/exe

1 >= /bin/sh

%%

$ readlink /proc/self/exe

1 >= /bin/readlink

%%

$ perl -e 'print readlink( "/proc/self/exe" ) . "\n"'

1 >= /usr/bin/perl

