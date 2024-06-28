#!/usr/bin/env jtest

$ vc 'abs 0'
1 >= 0

$ vc 'abs 8'
1 >= 8

$ vc 'abs (-8)'
1 >= 8

$ vc 'half 0'
1 >= 0

$ vc 'half 8'
1 >= 4

$ vc 'half 7'
1 >= 3

$ vc 'half (-8)'
1 >= -4

$ vc 'half (-7)'
1 >= -3

$ vc 'half 2^100 == 2^99'
1 >= true

$ vc 'half == Math.half'
1 >= true

$ vc 'Math.min(), Math.max()'
1 >= '()'

$ vc 'const P = [42, 512, 1729]; Math.min P, Math.max P'
1 >= '([42, 512, 1729], [42, 512, 1729])'

$ vc 'const P = 3, 1, 4, 1, 5, 9, 2, 6; Math.min P, Math.max P'
1 >= '(1, 9)'

$ vc 'const P = "foo", "bar", "baz"; Math.min P, Math.max P'
1 >= '("bar", "foo")'

$ vc 'const P = -7, -5, -3, -1; Math.min P, Math.max P'
1 >= '(-7, -1)'

$ vc 'const P = 5, 4, 3, 2, 1, 0; Math.min P, Math.max P'
1 >= '(0, 5)'

$ vc 'const P = 2, 2; Math.sum P, Math.product P'
1 >= '(4, 4)'

$ vc 'const P = 3, 3/2; Math.sum P, Math.product P'
1 >= '((9/2), (9/2))'

$ vc 'const P = 12, 144, 20; Math.sum P, Math.product P'
1 >= '(176, 34560)'

$ vc 'const P = 1, 2, 3, 4, 5; Math.sum P, Math.product P'
1 >= '(15, 120)'
