#!/usr/bin/env jtest

$ jtest /dev/fd/0

0 << 'INPUT'
$ true
INPUT

1 >> OUTPUT
1..1
ok 1
OUTPUT

%%

$ jtest /dev/fd/0

0 << 'INPUT'
$ true
%%
$ true
%%
$ true
INPUT

1 >> OUTPUT
1..3
ok 1
ok 2
ok 3
OUTPUT

