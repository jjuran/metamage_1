#!/usr/bin/env jtest

$ jtest /dev/fd/0

0 << 'INPUT'
$ false
TODO foo
INPUT

1 >> OUTPUT
1..1
not ok 1 # TODO foo
OUTPUT

%%

$ jtest /dev/fd/0

0 << 'INPUT'
$ true
TODO foo
INPUT

1 >> OUTPUT
1..1
ok 1 # TODO foo
OUTPUT

