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

%

$ vc '{123; 456}'
1 >= '{456}'

%

$ vc '{do {789}}'
1 >= '{789}'

%

$ vc '{assert 2 + 2 == 4}'
1 >= '{}'

%

$ vc '{const company = 2; const crowd = 3; 4 + 5}'
1 >= '{9}'

%

$ vc '{const company = 2; const crowd = 3; company + crowd}'
1 >= '{5}'

%

$ vc '{const x = 3; {const y = 4; {x + y}}}'
1 >= '{{{7}}}'

%

$ vc '{(+) [1, 2, 3]}'
1 >= '{1}'

%

$ vc '{(+) str^[foo: 1, bar: 2]}'
1 >= '{1}'

%

$ vc '{const m = str^[foo: 1, bar: 2, baz: 3]; m["bar"]}'
1 >= '{2}'

%

$ vc '{-[1, 2, 3, 4]}'
1 >= '{[4, 3, 2, 1]}'

%

$ vc '{[1, 2, 3].length}'
1 >= '{3}'

%

$ vc '{"foo" "bar"}'
1 >= '{"foobar"}'

%

$ vc '{const abc = "abc"; abc(123)}'
1 >= '{"abc123"}'

%

$ vc '{u8 .-1}'
1 >= '{255}'

%

$ vc '{x"4e75"}'
1 >= '{x"4e75"}'

%

$ vc '{Math.sum(3 + 4)}'
1 >= '{7}'

%

$ vc '{join("-", 1, 2, 3)}'
1 >= '{"1-2-3"}'
