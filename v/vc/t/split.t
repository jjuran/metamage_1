#!/usr/bin/env jtest

$ vc '"foo bar" / *" "'
1 >= '("foo", "bar")'

%

$ vc '"hello" / *"l"'
1 >= '("he", "", "o")'

%

$ vc '"hello" / *"h"'
1 >= '("", "ello")'

%

$ vc '"hello" / *"o"'
1 >= '("hell", "")'

%

$ vc '"hello" / *"?"'
1 >= '"hello"'

%

$ vc '"" / *"?"'
1 >= '""'

%

$ vc '"foo bar baz" / " ba"'
1 >= '("foo", "r", "z")'

%

$ vc '"foo bar baz" / "az"'
1 >= '("foo bar b", "")'

%

$ vc '"foo bar baz" / "f"'
1 >= '("", "oo bar baz")'

%

$ vc '"foo bar baz" / "qux"'
1 >= '"foo bar baz"'

%

$ vc '"foo bar baz" / "foo bar baz"'
1 >= '("", "")'

%

$ vc 'try { "foo bar baz" / "" } catch { "qux" }'
1 >= '"qux"'

%

$ vc '"foo bar baz" / x"61"'
1 >= '("foo b", "r b", "z")'

%

$ vc '"foo bar baz" / x"6261"'
1 >= '("foo ", "r ", "z")'

%

$ vc 'x"12345678" / "4"'
1 >= '(x"12", x"5678")'

%

$ vc 'x"12345678" / x"56"'
1 >= '(x"1234", x"78")'
