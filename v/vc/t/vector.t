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
1 >= 'vector[record ("size" => 4)]'

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

%

$ vc 'vector isa type'
1 >= true

%

$ vc 'vector[ record(size: 1) ] isa type'
1 >= true

%

$ vc 'vector == vector[ record(size: 1) ]'
1 >= false

%

$ vc '1 .. 2 map { record(size: _) } map vector'
1 >= '[vector[record ("size" => 1)], vector[record ("size" => 2)]]'

%

$ vc 'typeof vector[ u8 ] 4'
1 >= 'vector[u8]'

%

$ vc 'bool vector[ u8, 4 ]'
1 >= true

%

$ vc 'typeof vector[ u8, 4 ]'
1 >= type

%

$ vc 'vector[ u8, 4 ] isa type'
1 >= true

%

$ vc 'vector[ u8, 4 ]()'
1 >= '<vector>'

%

$ vc 'packed vector[ u8, 4 ]()'
1 >= 'x"00000000"'

%

$ vc 'packed vector[ u8, 4 ] x"00000001"'
1 >= 'x"00000001"'

%

$ vc 'bool vector[ u8, 4 ] x"00000000"'
1 >= false

%

$ vc 'bool vector[ u8, 4 ] x"00000001"'
1 >= true

%

$ vc 'vector[ u8, 4 ]() == vector[ u8, 4 ] x"00000000"'
1 >= true

%

$ vc 'packed vector[ u8, 0 ]()'
1 >= 'x""'

%

$ vc 'var v = vector[ u8, 4 ](); v[0] = 123; packed v'
1 >= 'x"7b000000"'

%

$ vc 'var v = vector[ u8, 4 ] x"12345678"; v[3]'
1 >= 120
