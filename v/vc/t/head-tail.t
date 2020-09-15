#!/usr/bin/env jtest

$ vc 'head ()'
1 >= '()'

%

$ vc 'tail ()'
1 >= '()'

%

$ vc 'head 123'
1 >= 123

%

$ vc 'tail 123'
1 >= '()'

%

$ vc 'head null'
1 >= null

%

$ vc 'tail null'
1 >= '()'

%

$ vc 'head (123, 456)'
1 >= 123

%

$ vc 'tail (123, 456)'
1 >= 456

%

$ vc 'head (123, 456, 789)'
1 >= 123

%

$ vc 'tail (123, 456, 789)'
1 >= '(456, 789)'

%

$ vc 'head [123, 456]'
1 >= '[123, 456]'

%

$ vc 'tail [123, 456]'
1 >= '()'

%

$ vc 'const x = "foo", "bar"; head x'
1 >= '"foo"'

%

$ vc 'const x = "foo", "bar"; tail x'
1 >= '"bar"'

%

$ vc 'const x = "foo", "bar"; head (x, "baz")'
1 >= '"foo"'

%

$ vc 'const x = "foo", "bar"; tail (x, "baz")'
1 >= '("bar", "baz")'

%

$ vc '(head % 1 % 2 % 3)()'
1 >= 1

%

$ vc '(tail % 1 % 2 % 3)()'
1 >= '(2, 3)'
