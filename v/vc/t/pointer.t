#!/usr/bin/env jtest

$ vc 'begin "Hello"'
1 >= '<pointer>'

%

$ vc '*begin "Hello"'
1 >= "'H'"

%

$ vc 'bool begin "Hello"'
1 >= true

%

$ vc 'begin ""'
1 >= '<pointer>'

%

$ vc 'bool begin ""'
1 >= false

%

$ vc 'var p = begin "Hello"; *p'
1 >= "'H'"

%

$ vc 'var p = begin "Hello"; ++p; *p'
1 >= "'e'"

%

$ vc 'var p = begin "Hello"; ++p; ++p; ++p; ++p; *p'
1 >= "'o'"

%

$ vc 'var p = begin "Hello"; ++p; ++p; ++p; ++p; *++p'
1 >= '()'

%

$ vc '*begin x"4e75"'
1 >= "'N'"

%

$ vc 'var p = begin "abcdef"; var q = ++p; ++q; ++q; ++q - p'
1 >= '"bcd"'

%

$ vc 'var p = begin "abc"; var q = ++p; ++q; ++q; try {p - q} catch {"nope"}'
1 >= '"nope"'

%

$ vc 'var p = begin "abcdef"; for i in 0 -> 3 do {p += *"a" .. *"b"}; p.rest'
1 >= '"cdef"'
