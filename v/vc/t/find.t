#!/usr/bin/env jtest

$ vc '(begin "abcde").find( *"a" ).rest'
1 >= '"abcde"'

%

$ vc '(begin "abcde").find( *"c" ).rest'
1 >= '"cde"'

%

$ vc '(begin "abcde").find( *"e" ).rest'
1 >= '"e"'

%

$ vc '(begin "abcde").find( *"z" )'
1 >= '()'

%

$ vc '(begin "foo").find( *"o" ).rest'
1 >= '"oo"'
