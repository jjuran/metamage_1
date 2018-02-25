#!/usr/bin/env jtest

$ vc 'def make-repf { const f = _; { rep f } }; const r = make-repf {...}; r()'
1 >= '"{...}"'

%

$ vc 'def mr { const f = _; { rep f } }; [mr {}, mr {123}, mr {}][1]()'
1 >= '"{123}"'

%

$ vc '(str * int) true'
1 >= '"1"'

%

$ vc 'const quarter = half * half; quarter 100'
1 >= 25

%

$ vc '(unhex * {"0x" _} * hex) x"4e75"'
1 >= 20085
