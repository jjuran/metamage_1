#!/usr/bin/env jtest

$ damogran-test-pack
1 >> ""

$ damogran-test-pack ""
1 >= ""

$ damogran-test-pack 00
1 >> ""

$ damogran-test-pack 0000
1 >= 00ff0000

$ damogran-test-pack 000000
1 >> ""

$ damogran-test-pack 00000000
1 >= 0100

$ damogran-test-pack 000000000000
1 >= 0200

$ damogran-test-pack 0123456789abcdef
1 >= 00fc0123456789abcdef
