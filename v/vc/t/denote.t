#!/usr/bin/env jtest

$ vc 'var x, var y = 1, 2'
1 >= '(1, 2)'

$ vc 'var x (int), var y = 1, 2'
1 >= '(1, 2)'

$ vc 'var x, var y (int) = 1, 2'
1 >= '(1, 2)'

$ vc 'var x (int), var y (int) = 1, 2'
1 >= '(1, 2)'

$ vc 'var x (str), var y (bool) := 1, 2'
1 >= '("1", true)'

$ vc 'var x (str), var y (...) = "-", 1, 2, 3; join( x, y )'
1 >= '"1-2-3"'

$ vc 'var x, ... = 1, 2, 3'
1 >= 1

$ vc 'var x (str), var y (...) = "solo"; [x], [y]'
1 >= '(["solo"], [])'

$ vc 'var x (str), ... = "solo"'
1 >= '"solo"'

$ vc 'var x (V.one) = 1'
1 >= 1

$ vc 'var x (V.one), var y (V.one) = 1, 2'
1 >= '(1, 2)'

$ vc 'try {var x (V.one) = 1, 2} catch {"nope"}'
1 >= '"nope"'

$ vc 'try {var x (V.one) = ()} catch {"nope"}'
1 >= '"nope"'
