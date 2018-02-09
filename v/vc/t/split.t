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
