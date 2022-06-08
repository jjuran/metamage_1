#!/usr/bin/env jtest

$ vx -e 'print rep (["", ".", "..", "foo", "foo//"] map dirname)'
1 >= '[".", ".", ".", ".", "."]'

%

$ vx -e 'print rep (["/", "//", "///", "/foo", "/foo//"] map dirname)'
1 >= '["/", "/", "/", "/", "/"]'

%

$ vx -e 'print rep (["foo/bar", "foo/bar//"] map dirname)'
1 >= '["foo", "foo"]'

%

$ vx -e 'print rep (["/foo/bar", "/foo/bar//"] map dirname)'
1 >= '["/foo", "/foo"]'

%

$ vx -e 'print rep {dirname "foo/bar"}'
1 >= '{"foo"}'
