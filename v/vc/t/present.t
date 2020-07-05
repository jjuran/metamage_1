#!/usr/bin/env jtest

$ vc '()?'
1 >= '()'

%

$ vc '123?, "hi"?, true?'
1 >= '(123, "hi", true)'

%

$ vc 'const x = 123; x?, xx?, (x + 1)?'
1 >= '(123, null, 124)'

%

$ vc 'var x = 123; x?, xx?'
1 >= '(123, null)'

%

$ vc 'const x = 123; { const y = 456; x?, y?, z?, (x + y)? }'
1 >= '{123, 456, null, 579}'

%

$ vc 'var x = 123; { x?, xx? }'
1 >= '{x, null}'

%

$ vc 'join? (" ", 1, 2, 3)'
1 >= '"1 2 3"'

%

$ vc 'enjoin? (" ", 1, 2, 3)'
1 >= null

%

$ vc 'if mysql_real_escape_string? then {mysql_real_escape_string "foo"}'
1 >= '()'

%

$ vc 'def f {"foo"}; f?()'
1 >= '"foo"'
