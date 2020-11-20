#!/usr/bin/env jtest

$ vc 'for i in 1 .. 3 do {var x = 0}; .culls V.tracker'
1 >= 3
