#!/usr/bin/env jtest

$ vx -e 'print rep argv' foo bar baz
1 >= '["-e", "foo", "bar", "baz"]'

$ vx v/examples/argv.vx foo bar baz
1 >= '["v/examples/argv.vx", "foo", "bar", "baz"]'

$ vx < v/examples/argv.vx
1 >= '["-"]'
