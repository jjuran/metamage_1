#!/usr/bin/env jtest

$ bits2pict-test
1 >> ""

$ bits2pict-test ""
1 >= ""

$ bits2pict-test b175000000000000
1 >= ""

$ bits2pict-test b175000100010001aa
1 >= ""

$ bits2pict-test b175000200010001aa
1 >= ""

$ bits2pict-test b175000200020001aabbcc
1 >= ""
