#!/usr/bin/env jtest

$ vc '*int'
1 >= '(*integer)'

$ vc '*(int[])'
1 >= '(*(integer[]))'

$ vc '**int'
1 >= '(*(*integer))'

$ vc '*...'
1 >= '(*...)'

$ vc 'typeof (*int)'
1 >= type

$ vc 'var x (*...), var y (**int)'
1 >= '(x, y)'

$ vc 'var x; &x'
1 >= '(&x)'

$ vc 'var x; &x == &x'
1 >= true

$ vc 'var x; var a = &x; a'
1 >= '(&x)'

$ vc 'var x; var a = &x; var b = &x; a == b'
1 >= true

$ vc 'var x; var a = &x; {var x; var b = &x; a, b}()'
1 >= '((&x), (&x))'

$ vc 'var x; var a = &x; {var x; var b = &x; a == b}()'
1 >= false

$ vc 'var x = [1, 2, 3]; &x[1]'
1 >= '(&(x[1]))'

$ vc 'var x = [1, 2, 3]; typeof (&x[1], &x)'
1 >= '((*...), (*...))'

$ vc 'var x (...[]) = [1, 2, 3]; typeof (&x[1], &x)'
1 >= '((*...), (*(...[])))'

$ vc 'var x (int[]) = [1, 2, 3]; typeof (&x[1], &x)'
1 >= '((*integer), (*(integer[])))'

$ vc 'var x (u8[]) = [1, 2, 3]; typeof (&x[1], &x)'
1 >= '((*u8), (*(u8[])))'

$ vc 'var x = 123; var y (*int) = &x'
1 >= '(&x)'

$ vc 'var x = 123; var y (*int) = &x; var z (**int) = &y'
1 >= '(&y)'

$ vc 'var x = 12345; try { var y (*u8) = &x } catch {"nope"}'
1 >= '"nope"'

$ vc 'var x = 123; var y (*int) = &x; try { y = &y } catch {"nope"}'
1 >= '"nope"'

$ vc 'var x = 123; var y (*int) = &x; try { var z (**int) = &x } catch {"nope"}'
1 >= '"nope"'

$ vc 'var x (*...) = &x'
1 >= '(&x)'

$ vc 'var x = 123; var y (*int) = &x; *y'
1 >= '123'

$ vc 'var x = [1, 2, 3]; var y = &x; **y'
1 >= '(1, 2, 3)'

$ vc 'var x = 123; var y = &x; var z = &y; z, *z, **z'
1 >= '((&y), (&x), 123)'

$ vc 'var x (*...) = &x; x, *x, **x'
1 >= '((&x), (&x), (&x))'

$ vc 'var x = [1, 2, 3]; var y = &x; *y = 4; x'
1 >= 4

$ vc 'var x = [1, 2, 3]; var y = &x; *y <-- 4; x'
1 >= '[1, 2, 3, 4]'

$ vc 'var x = [1, 2, 5]; var y = &x[2]; *y = 3; x'
1 >= '[1, 2, 3]'
