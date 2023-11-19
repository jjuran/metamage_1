#!/usr/bin/env jtest

$ vc 'def f(x) {let y = 0; let z = x}; f 3, f 4'
1 >= '(3, 4)'
