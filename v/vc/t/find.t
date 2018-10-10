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

%

$ vc '(begin "abcde").find( "abc" ).rest'
1 >= '"abcde"'

%

$ vc '(begin "abcde").find( "bcd" ).rest'
1 >= '"bcde"'

%

$ vc '(begin "abcde").find( "e" ).rest'
1 >= '"e"'

%

$ vc '(begin "abcde").find( "" ).rest'
1 >= '"abcde"'

%

$ vc '(begin "abcde").find( "abcde" ).rest'
1 >= '"abcde"'

%

$ vc '(begin "abcde").find( "def" )'
1 >= '()'

%

$ vc '(begin x"4e714e75").find( "Nu" ).past.string'
1 >= '"Nq"'

%

$ vc '(begin "foo\n\tbar").find( x"0a09" ).rest'
1 >= '"\n\tbar"'

%

$ vc '(begin x"07ffffe0").find( x"ffff" ).past'
1 >= 'x"07"'

%

$ vc '(begin "abcde").find( *"c" .. *"d" ).rest'
1 >= '"cde"'
