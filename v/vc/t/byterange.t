#!/usr/bin/env jtest

$ vc "packed('a' .. 'e').string"
1 >= '"abcde"'

%

$ vc "packed('a' .. 'a').string"
1 >= '"a"'

%

$ vc "try {'a' -> 'e'} catch {-50}"
1 >= '-50'

%

$ vc "packed('\0' .. '\xFF').string.length"
1 >= 256

%

$ vc "packed('A' .. 'C', 'a' .. 'c').string"
1 >= '"ABCabc"'

%

$ vc "['A', 'b', '3', 'D', 'e', 'f', '.'] ver {v in 'a' .. 'e'} per str"
1 >= '"be"'

%

$ vc "string('a' .. 'e')"
1 >= '"abcde"'
