#!/usr/bin/env jtest

$ vc '(1.0f/0, -1.0f/0, 0.0f/0)'
1 >= '(Inf, -Inf, NaN)'

$ vc '0.0f == -0.0f'
1 >= true

$ vc 'Iter.popcount [-0.0f, 0.0f]'
1 >= 0

$ vc 'Iter.popcount [-1.0f/0, 1.0f/0]'
1 >= 2

$ vc '2.0f^2^7'
1 >= 340282366920938463463374607431768211456.000000
