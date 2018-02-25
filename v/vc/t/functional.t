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

%

$ vc 'const eighth = half^3; eighth 40'
1 >= 5

%

$ vc 'sha256^2^8 ""'
1 >= 'x"d6481c92fd3d6f5a0f215c5eef33ebbb84017abab5247114729abf7f7eb93690"'

%

$ vc '{ join( ".", *_ ) }^2 "abcd"'
1 >= '"a...b...c...d"'

%

$ vc '{ join( ".", *_ ) }^1 "abcd"'
1 >= '"a.b.c.d"'

%

$ vc '{ join( ".", *_ ) }^0 "abcd"'
1 >= '"abcd"'
