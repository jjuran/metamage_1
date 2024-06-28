#!/usr/bin/env jtest

$ vx -e 'print rep getenv "This environment variable is almost certainly not defined."'
1 >= "()"

$ FOO=foo vx -e 'print rep getenv "FOO"'
1 >= '"foo"'

$ FOO="" vx -e 'print rep getenv "FOO"'
1 >= '""'

$ FOO=BAR BAR=BAZ vx -e 'print getenv getenv "FOO"'
1 >= BAZ

$ THREE=3 vx -e ' print getenv("THREE").length'
1 >= 1
