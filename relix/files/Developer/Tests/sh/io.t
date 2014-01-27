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

%%

$ sh -c 'echo foo > "/dev/fd/1"' 2>&1

1 >= foo

%%

$ echo false | sh -c '. /dev/fd/0; echo $?'

1 >= 1

%%

$ echo "FOO=bar; echo foo" | sh -c '. /dev/fd/0 > /dev/null; echo $FOO'

1 >> 'END'
bar
END

%%

$ echo "FOO=bar; echo foo" | sh -c '. /dev/fd/0 | true; echo $FOO'

1 >> 'END'

END

%%

$ echo false | sh -c '. /dev/fd/0 2>&1; echo $?'

1 >= 1

