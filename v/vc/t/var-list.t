#!/usr/bin/env jtest

$ vc 'var (x, y)'
1 >= '(x, y)'

%

$ vc 'var (x, y) = 1, 2'
1 >= '(1, 2)'

%

$ vc 'var (x, y) = 1, 2; x, y'
1 >= '(1, 2)'

%

$ vc 'var (x, y), var z = 1, 2, 3; x, y, z'
1 >= '(1, 2, 3)'

%

$ vc 'var x, var (y, z) = 1, 2, 3; x, y, z'
1 >= '(1, 2, 3)'

%

$ vc 'let (x, y)'
1 >= '(x, y)'

%

$ vc 'let (x, y) = 1, 2'
1 >= '(1, 2)'

%

$ vc 'let (x, y) = 1, 2; x, y'
1 >= '(1, 2)'

%

$ vc 'let (x, y), let z = 1, 2, 3; x, y, z'
1 >= '(1, 2, 3)'

%

$ vc 'let x, let (y, z) = 1, 2, 3; x, y, z'
1 >= '(1, 2, 3)'
