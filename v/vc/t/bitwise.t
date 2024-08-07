#!/usr/bin/env jtest

$ vc '~x""'
1 >= 'x""'

$ vc 'x"" & x""'
1 >= 'x""'

$ vc 'x"" | x""'
1 >= 'x""'

$ vc 'x"" ~ x""'
1 >= 'x""'

$ vc '~x"1234567890bfaced"'
1 >= 'x"edcba9876f405312"'

$ vc 'x"1234567890bfaced" & x"5566778855667788"'
1 >= 'x"1024560810262488"'

$ vc 'x"1234567890bfaced" | x"5566778855667788"'
1 >= 'x"577677f8d5ffffed"'

$ vc 'x"1234567890bfaced" ~ x"5566778855667788"'
1 >= 'x"475221f0c5d9db65"'

$ vc -- '-x"1234" x"5678"'
1 >= 'x"34125678"'

$ vc 'var x = x"12345678"; x &= x"66" * 4'
1 >= 'x"02244660"'

$ vc 'var x = x"12345678"; x |= x"66" * 4'
1 >= 'x"7676767e"'

$ vc 'var x = x"12345678"; x ~= x"66" * 4'
1 >= 'x"7452301e"'
