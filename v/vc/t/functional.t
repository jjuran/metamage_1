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

%

$ vc 'md5^1 ""'
1 >= 'x"d41d8cd98f00b204e9800998ecf8427e"'

%

$ vc 'md5^5 ""'
1 >= 'x"b3786b60f640690c03030ebcff4bdf8d"'

%

$ vc '*{}'
1 >= '()'

%

$ vc '*{[]}'
1 >= '[]'

%

$ vc '*{123}'
1 >= 123

%

$ vc '*{1, 2, 3}'
1 >= '(1, 2, 3)'

%

$ vc '{*{}}'
1 >= '{}'

%

$ vc '{*{null}}'
1 >= '{null}'

%

$ vc 'const x = 3; {*{x}}'
1 >= '{3}'

%

$ vc 'var x = 3; {*{x}}'
1 >= '{*{x}}'

%

$ vc '{*{break}}'
1 >= '{*{break}}'

%

$ vc '()()'
1 >= '()'

%

$ vc '() 123'
1 >= '()'

%

$ vc '() 123'
1 >= '()'

%

$ vc '[(md5, sha256, ...) "greetings"] map {v[ 0 -> 8 ]}'
1 >= '[x"f4a04f87cabf65ed", x"7dd4f2f077e449b4", "greeting"]'

%

$ vc '["We hold these truths to be self-evident" / *" "] map .length'
1 >= '[2, 4, 5, 6, 2, 2, 12]'

%

$ vc '(.key, .value)(foo: 1)'
1 >= '("foo", 1)'

%

$ vc '(.keys, .values) str^[foo: 1, bar: 2]'
1 >= '(["foo", "bar"], [1, 2])'

%

$ vc '.value^2 (foo: bar: "baz")'
1 >= '"baz"'

%

$ vc '(.y, .x, .dy, .dx) record(x: 0, y: 20, dx: 512, dy: 322)'
1 >= '(20, 0, 322, 512)'
