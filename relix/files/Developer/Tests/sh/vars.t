#!/usr/bin/env jtest

# set shell variable only
$ FOO=bar; echo $FOO

1 >= bar

%%

# set shell variable only
$ FOO=bar; sh -c 'echo $FOO'

1 >= ""

%%

# set shell variable, then export
$ FOO=bar; export FOO; echo $FOO

1 >= bar

%%

# set shell variable, then export
$ FOO=bar; export FOO; sh -c 'echo $FOO'

1 >= bar

%%

# set environment variable
$ export FOO=bar; echo $FOO

1 >= bar

%%

# set environment variable
$ export FOO=bar; sh -c 'echo $FOO'

1 >= bar

%%

# set environment variable en passant
$ FOO=bar sh -c 'echo $FOO'

1 >= bar

%%

# set multiple shell variables
$ FOO=foo BAR=bar; echo $FOO $BAR

1 >= "foo bar"

%%

# set multiple environment variables en passant
$ FOO=foo BAR=bar sh -c 'echo $FOO $BAR'

1 >= "foo bar"

%%

# set multiple environment variables en passant in a pipeline
$ true | FOO=foo BAR=bar sh -c 'echo $FOO $BAR'

1 >= "foo bar"

