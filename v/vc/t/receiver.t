#!/usr/bin/env jtest

$ vc 'receiver {}'
1 >= '<receiver>'

$ vc '<=receiver {}'
1 >= '()'

$ vc 'var x = 0; const r = receiver {++x}; <=r'
1 >= '1'

$ vc 'var x = 0; const r = receiver {++x}; <=r; <=r'
1 >= '2'

$ vc 'var y = []; var n = 3; for x in receiver {n-- or ()} do {y <-- x}; y'
1 >= '[3, 2, 1]'
