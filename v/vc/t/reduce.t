#!/usr/bin/env jtest

$ vc '[] per {throw 0}'
1 >= '()'

$ vc '[123] per {throw 0}'
1 >= '123'

$ vc 'try {[] per "not a function"} catch {"nope"}'
1 >= '"nope"'

$ vc '[12, 144, 20] per Math.sum'
1 >= 176

$ vc '[12, 144, 20] per Math.max'
1 >= 144

$ vc '2 .. 5 per Math.product'
1 >= 120

$ vc '2 .. 1 per Math.product'
1 >= '()'

$ vc '"Hello" per ...'
1 >= "('H', 'e', 'l', 'l', 'o')"

$ vc '"foo bar" per {a and b}'
1 >= "'r'"

$ vc '"foo\x00bar" per {a and b}'
1 >= "'\x00'"

$ vc '"\x00foo\0" per {a or b}'
1 >= "'f'"

$ vc '[ x"0003", x"0100", x"0400" ] per {a | b}'
1 >= 'x"0503"'

$ vc '[ "foo", "bar", "baz" ] per str'
1 >= '"foobarbaz"'

$ vc '8 .. 12 map ([ {_ / 2}, str, {_ * 2} ] per { var f, var g = _; {f g _} })'
1 >= '["1", "6", "1", "8", "2", "0", "2", "2", "2", "4"]'

$ vc '8 .. 12 map ([ {_ / 2}, str, {_ * 2} ] per { var f, var g = _; {g f _} })'
1 >= '["44", "9/29/2", "55", "11/211/2", "66"]'

$ vc '2 .. 1 per 1 >-Math.product'
1 >= 1

$ vc '["one", "two", "three", "four"] per 0 >-{L + v.length}'
1 >= 15

$ vc '1 .. 5 per [1, 1]>-{[L[1], L per {a + b}]}'
1 >= '[8, 13]'
