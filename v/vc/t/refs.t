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
