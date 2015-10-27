#!/usr/bin/env jtest

$ vc 0o0
1 >= 0

$ vc 0o1
1 >= 1

$ vc 0o7
1 >= 7

$ vc 0o10
1 >= 8

$ vc 0o100
1 >= 64

$ vc 0o377
1 >= 255
