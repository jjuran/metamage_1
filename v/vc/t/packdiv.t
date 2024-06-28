#!/usr/bin/env jtest

$ vc 'x"" / 1'
1 >= 'x""'

$ vc 'x"" / -1'
1 >= '()'

$ vc 'x"" / 2'
1 >= '(x"", x"")'

$ vc 'x"" / -2'
1 >= '()'

$ vc 'x"42" / 1'
1 >= 'x"42"'

$ vc 'x"42" / -1'
1 >= 'x"42"'

$ vc 'x"cafe" / 1'
1 >= 'x"cafe"'

$ vc 'x"cafe" / -1'
1 >= '(x"ca", x"fe")'

$ vc 'x"cafe" / 2'
1 >= '(x"ca", x"fe")'

$ vc 'x"cafe" / -2'
1 >= 'x"cafe"'

$ vc 'x"decade0fbeef" / 1'
1 >= 'x"decade0fbeef"'

$ vc 'x"decade0fbeef" / -1'
1 >= '(x"de", x"ca", x"de", x"0f", x"be", x"ef")'

$ vc 'x"decade0fbeef" / 2'
1 >= '(x"decade", x"0fbeef")'

$ vc 'x"decade0fbeef" / -2'
1 >= '(x"deca", x"de0f", x"beef")'

$ vc 'x"decade0fbeef" / 3'
1 >= '(x"deca", x"de0f", x"beef")'

$ vc 'x"decade0fbeef" / -3'
1 >= '(x"decade", x"0fbeef")'

$ vc 'x"decade0fbeef" / 6'
1 >= '(x"de", x"ca", x"de", x"0f", x"be", x"ef")'

$ vc 'x"decade0fbeef" / -6'
1 >= 'x"decade0fbeef"'
