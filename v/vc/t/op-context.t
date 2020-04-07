#!/usr/bin/env jtest

$ vc 'hex ~x"abc123"'
1 >= '"543edc"'

%

$ vc 'const x = 123; :x'
1 >= '("x" => 123)'

%

$ vc 'const x = 123; rep :x'
1 >= '"(\"x\" => 123)"'
