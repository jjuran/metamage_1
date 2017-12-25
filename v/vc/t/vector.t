#!/usr/bin/env jtest

$ vc 'vector, str vector'
1 >= '(vector, "vector")'

%

$ vc 'typeof vector'
1 >= type

%

$ vc 'bool vector'
1 >= true

%

$ vc 'vector[ record(size: 4) ]'
1 >= '<vector type>'

%

$ vc 'typeof vector[ record(size: 4) ]'
1 >= type

%

$ vc 'bool vector[ record(size: 4) ]'
1 >= true

%

$ vc 'try { vector[ record(size: 0) ] 0 } catch { "divided by zero" }'
1 >= '"divided by zero"'

%

$ vc 'packed vector[ record(size: 2) ] 3'
1 >= x"000000000000"

%

$ vc 'bool vector[ record(size: 2) ] 3'
1 >= true

%

$ vc 'bool vector[ record(size: 2) ] 0'
1 >= false

%

$ vc 'vector[ record(size: 4) ] isa vector'
1 >= false

%

$ vc 'vector[ record(size: 2) ] 3'
1 >= '<vector>'

%

$ vc 'vector[ record(size: 2) ] 3 isa vector'
1 >= true

%

$ vc 'vector[ record(size: 2) ] 3 isa vector[ record(size: 2) ]'
1 >= true

%

$ vc 'vector[ record(size: 2) ] 3 isa vector[ record(size: 4) ]'
1 >= false

%

$ vc 'try { var v (vector) = vector[ record(size: 2) ] } catch { "nope" }'
1 >= '"nope"'

%

$ vc 'var v (vector) = vector[ record(size: 2) ] 3'
1 >= '<vector>'

%

$ vc 'const T = vector[ record(size: 2) ]; var x (T) = T 3'
1 >= '<vector>'

%

$ vc 'var v = vector[ record(size: 1, encode: {~_}) ] 2; v[1] = x"7e"; packed v'
1 >= x"0081"

%

$ vc 'var v = vector[ record(size: 2, decode: {~_}) ] x"12345678"; *v'
1 >= '(x"edcb", x"a987")'

%

$ vc 'var v (vector[ record(size: 2, decode: {~_}) ]); v := x"deadbeef"; *v'
1 >= '(x"2152", x"4110")'
