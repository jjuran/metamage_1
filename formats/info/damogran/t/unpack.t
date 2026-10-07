#!/usr/bin/env jtest

$ damogran-test-unpack
1 >> ""

$ damogran-test-unpack ""
1 >= ""

$ damogran-test-unpack 00
1 >> ""

$ damogran-test-unpack 0000
1 >= ""

$ damogran-test-unpack 000000
1 >> ""

$ damogran-test-unpack 00000000
1 >= ""

$ damogran-test-unpack 00ff0000
1 >= 0000

$ damogran-test-unpack 0100
1 >= 00000000

$ damogran-test-unpack 0200
1 >= 000000000000

$ damogran-test-unpack 00fc0123456789abcdef
1 >= 0123456789abcdef
