#!/usr/bin/env jtest

$ perl -e 'print "foo", "\n"'

1 >= foo

%%

$ perl -le 'print "foo"'

1 >= foo

%%

$ perl -e 'print STDERR "foo", "\n"'

1 >> ""

2 >= foo

