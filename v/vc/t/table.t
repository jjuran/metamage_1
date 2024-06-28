#!/usr/bin/env jtest

$ vc 'var t = str^[foo: 1, bar: 2]; t <-- baz: 3; t'
1 >= '(string^[("foo" => 1), ("bar" => 2), ("baz" => 3)])'

$ vc 'var t = str^[foo: 1, bar: 2]; t <-- (); t'
1 >= '(string^[("foo" => 1), ("bar" => 2)])'

$ vc 'var t = str^[]; t <-- baz: 3; t'
1 >= '(string^["baz" => 3])'

$ vc 'var t = str^[]; t <-- (); t'
1 >= '(string^[])'

$ vc 'int^[] map {_}'
1 >= '[]'
