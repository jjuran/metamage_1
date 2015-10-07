#!/usr/bin/env jtest

$ vx -e 'print rep eval "()"'
1 >= '()'

%

$ vx -e 'print rep eval "3 + 4"'
1 >= '7'

%

$ vx -e 'print rep eval "{3 + 4}"'
1 >= '{3 + 4}'

%

$ vx -e 'const f = eval "{3 + 4}"; print rep f()'
1 >= '7'

%

$ vx -e 'const f = eval "var x = 0; {++x}"; print rep(f(), f(), f())'
1 >= '(1, 2, 3)'

%

$ vx -e 'const f = eval "{if _%2 then {3*_+1} else {_/2}}"; print rep(f 5, f 6)'
1 >= '(16, 3)'

%

$ vx -e 'const x = try { eval "\u2022" } catch {"bullet"}; print rep x;'
1 >= '"bullet"'

%

$ vx -e 'const x = try { eval "1 / 0" } catch {"divided by zero"}; print rep x;'
1 >= '"divided by zero"'

%

$ vx -e 'const x = try { eval "eval str 0" } catch {"nope"}; print rep x;'
1 >= '"nope"'
