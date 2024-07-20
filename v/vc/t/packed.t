#!/usr/bin/env jtest

$ vc 'var s = "bar"; s[ 2 ] = *"z"'
1 >= "'z'"

$ vc 'var s = "bar"; s[ 2 ] = *"z"; s'
1 >= '"baz"'

$ vc 'var x = x"4e75"; x[ 1 ] = byte 0x71; x'
1 >= 'x"4e71"'

$ vc 'var a = [ "No. 1:  The larch" ]; a[ 0 ][ 4 ] = *"3"; a'
1 >= '["No. 3:  The larch"]'

$ vc '"[" x"1234" "]"'
1 >= '"[1234]"'

$ vc 'mince x"1234567890"'
1 >= 'x"125690"'

$ vc 'byte string'
1 >= packed
