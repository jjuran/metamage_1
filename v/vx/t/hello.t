#!/usr/bin/env jtest

$ vx -e '"Hello world"'
1 >> .
.

%

$ vx -e 'print "Hello world"'
1 >= 'Hello world'
