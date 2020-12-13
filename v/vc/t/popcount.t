#!/usr/bin/env jtest

$ vc 'Iter.popcount []'
1 >= 0

%

$ vc 'Iter.popcount ""'
1 >= 0

%

$ vc 'Iter.popcount (1 -> 1)'
1 >= 0

%

$ vc 'Iter.popcount [-0.0f, null, 0.0f]'
1 >= 0

%

$ vc 'Iter.popcount [2, 4, 0, -12, -5]'
1 >= 4

%

$ vc 'Iter.popcount "foo\x00bar\0"'
1 >= 6

%

$ vc 'Iter.popcount (-1 .. 5)'
1 >= 6

%

$ vc 'Iter.popcount md5 ""'
1 >= 15

%

$ vc 'Iter.popcount [[], [[]]]'
1 >= 1
