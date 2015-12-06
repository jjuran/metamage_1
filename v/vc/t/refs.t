#!/usr/bin/env jtest

$ vc '*int'
1 >= '(*integer)'

%

$ vc '*(int[])'
1 >= '(*(integer[]))'

%

$ vc '**int'
1 >= '(*(*integer))'

%

$ vc '*...'
1 >= '(*...)'

%

$ vc 'typeof (*int)'
1 >= type

%

$ vc 'var x (*...), var y (**int)'
1 >= '(x, y)'

%

$ vc 'var x; &x'
1 >= '(&x)'

%

$ vc 'var x; &x == &x'
1 >= true

%

$ vc 'var x; var a = &x; a'
1 >= '(&x)'

%

$ vc 'var x; var a = &x; var b = &x; a == b'
1 >= true

%

$ vc 'var x; var a = &x; {var x; var b = &x; a, b}()'
1 >= '((&x), (&x))'

%

$ vc 'var x; var a = &x; {var x; var b = &x; a == b}()'
1 >= false

%

$ vc 'var x = [1, 2, 3]; &x[1]'
1 >= '(&(x[1]))'

%

$ vc 'var x = [1, 2, 3]; typeof (&x[1], &x)'
1 >= '((*...), (*...))'

%

$ vc 'var x (...[]) = [1, 2, 3]; typeof (&x[1], &x)'
1 >= '((*...), (*(...[])))'

%

$ vc 'var x (int[]) = [1, 2, 3]; typeof (&x[1], &x)'
1 >= '((*integer), (*(integer[])))'

%

$ vc 'var x (u8[]) = [1, 2, 3]; typeof (&x[1], &x)'
1 >= '((*u8), (*(u8[])))'
