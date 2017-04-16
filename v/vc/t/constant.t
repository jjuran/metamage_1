#!/usr/bin/env jtest

$ vc '{3 + 4}'
1 >= '{7}'

%

$ vc '{2 + 2 == 4}'
1 >= '{true}'

%

$ vc '{3 + 1/2}'
1 >= '{(7/2)}'

%

$ vc '{"abcde"[2]}'
1 >= "{'c'}"

%

$ vc '{"george" in -"groe" * 3}'
1 >= '{true}'

%

$ vc '{(+) null}'
1 >= '{1}'
