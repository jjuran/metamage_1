#!/usr/bin/env jtest

$ vc 'is_pure ()'
1 >= 'true'

$ vc 'is_pure (1, "foo", rep, [], null)'
1 >= 'true'

$ vc '[translate, {}, {123}, {_^_}] ver is_pure'
1 >= '[{}, {123}, {_^_}]'

$ vc 'var x = 3; const y = 4; [{x}, {y}, {var x = 5; x + y}] ver is_pure'
1 >= '[{4}, {((var x) = 5); (x + 4)}]'

$ vc '[{var x = 3; &x}, {var x = 4; ++x}] ver is_pure'
1 >= '[{((var x) = 4); (++x)}]'
