#!/usr/bin/env jtest

$ jtest /dev/fd/0

0 << 'INPUT'
$ false
INPUT

1 >> OUTPUT
1..1
not ok 1
OUTPUT

%%

$ jtest /dev/fd/0

0 << 'INPUT'
$ true
%%
$ false
%%
$ false
INPUT

1 >> OUTPUT
1..3
ok 1
not ok 2
not ok 3
OUTPUT

%%

$ jtest /dev/fd/0

0 << 'INPUT'
$ false
%%
$ false
%%
$ true
INPUT

1 >> OUTPUT
1..3
not ok 1
not ok 2
ok 3
OUTPUT

