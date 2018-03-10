#!/usr/bin/env jtest

$ vc 'var x, var y = 1, 2'
1 >= '(1, 2)'

%

$ vc 'var x (int), var y = 1, 2'
1 >= '(1, 2)'

%

$ vc 'var x, var y (int) = 1, 2'
1 >= '(1, 2)'

%

$ vc 'var x (int), var y (int) = 1, 2'
1 >= '(1, 2)'

%

$ vc 'var x (str), var y (bool) := 1, 2'
1 >= '("1", true)'

%

$ vc 'var x (str), var y (...) = "-", 1, 2, 3; join( x, y )'
1 >= '"1-2-3"'
