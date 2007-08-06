#!/usr/bin/jtest

$ sh -c 'echo foo; echo bar'

1 >> '[END]'
foo
bar
[END]

