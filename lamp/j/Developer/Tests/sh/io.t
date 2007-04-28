#!/usr/bin/env jtest

$ /bin/echo foo

1 >= foo

2 >> 'NIL'
NIL

%%

$ /bin/echo foo >&2

1 >> 'NIL'
NIL

2 >= foo

%%

$ echo foo >&2

1 >> 'NIL'
NIL

2 >= foo

%%

$ /bin/echo foo >&9

? 1

1 >> 'NIL'
NIL

2 >> 'END'
sh: 9: Bad file descriptor
END

%%

$ /bin/echo foo >&3

? 1

1 >> 'NIL'
NIL

2 >> 'END'
sh: 3: Bad file descriptor
END

