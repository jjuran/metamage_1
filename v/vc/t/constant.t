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

%

$ vc '{const csv = join % ","; csv (1, 2, 3)}'
1 >= '{"1,2,3"}'

%

$ vc '{".".join(1, 2, 3)}'
1 >= '{"1.2.3"}'

%

$ vc '{"foo\nbar\n".lines()}'
1 >= '{["foo", "bar"]}'

%

$ vc '{const hash = hex * md5; hash "" }'
1 >= '{"d41d8cd98f00b204e9800998ecf8427e"}'

%

$ vc '{md5^2 ""}'
1 >= '{x"59adb24ef3cdbe0297f05b395827453f"}'

%

$ vc '{-2 .. 2 map abs}'
1 >= '{[2, 1, 0, 1, 2]}'

%

$ vc '{-3 .. 3 ver half}'
1 >= '{[-3, -2, 2, 3]}'

%

$ vc '{2 .. 5 per Math.product}'
1 >= '{120}'

%

$ vc '{2 .. 1 per 1>-Math.product}'
1 >= '{1}'

%

$ vc '{1 .. 3 map {v^2}}'
1 >= '{[1, 4, 9]}'

%

$ vc '{1 -> 1000 ver {v % 3 == 0 or v % 5 == 0} per {a + b}}'
1 >= '{233168}'

%

$ vc '{{3 * _ + 1} 5}'
1 >= '{16}'

%

$ vc '{{break} 2}'
1 >= '{{break} 2}'

%

$ vc '{{continue} 3}'
1 >= '{{continue} 3}'

%

$ vc '{{return _} 18}'
1 >= '{{return _} 18}'

%

$ vc '{{throw _} 20}'
1 >= '{{throw _} 20}'

%

$ vc '{{*_} "hi"}'
1 >= "{'h', 'i'}"

%

$ vc '{var x; {*_} .&x}'
1 >= '{(var x); ({*_} (&x))}'
