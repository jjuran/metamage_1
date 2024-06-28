#!/usr/bin/env jtest

$ vx -e '"Hello world"'
1 >> .
.

$ vx -e 'print "Hello world"'
1 >= 'Hello world'

$ vx -e 'def send (text) {OUT <== text "\n"}; print rep {send "Hello world"}'
1 >= '{send "Hello world"}'
