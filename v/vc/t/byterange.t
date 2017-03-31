#!/usr/bin/env jtest

$ vc "packed('a' .. 'e').string"
1 >= '"abcde"'

%

$ vc "packed('a' .. 'a').string"
1 >= '"a"'

%

$ vc "packed('a' -> 'e').string"
1 >= '"abcd"'

%

$ vc "packed('a' -> 'a').string"
1 >= '""'

%

$ vc "packed('\0' .. '\xFF').string.length"
1 >= 256

%

$ vc "packed('A' .. 'C', 'a' .. 'c').string"
1 >= '"ABCabc"'
