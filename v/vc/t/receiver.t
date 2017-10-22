#!/usr/bin/env jtest

$ vc 'receiver {}'
1 >= '<receiver>'

%

$ vc '<=receiver {}'
1 >= '()'

%

$ vc 'var x = 0; const r = receiver {++x}; <=r'
1 >= '1'

%

$ vc 'var x = 0; const r = receiver {++x}; <=r; <=r'
1 >= '2'
