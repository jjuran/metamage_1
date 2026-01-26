#!/usr/bin/env jtest

$ dec2str -d0 -e0 0
1 >= ' 0e+0'

$ dec2str -d0 -e0 -- -0
1 >= '-0e+0'

$ dec2str -d0 -e0 I
1 >= ' INF'

$ dec2str -d0 -e0 -- -I
1 >= -INF

$ dec2str -d0 -e0 N
1 >= ' NAN(000)'

$ dec2str -d0 -e0 -- -N
1 >= '-NAN(000)'

$ dec2str -d3 -e0 0
1 >= ' 0.00e+0'

$ dec2str -d3 -e2 0
1 >= ' 0.00e+0'

$ dec2str -d3 -e-2 0
1 >= ' 0.00e+0'

$ dec2str -d0 -e0 4
1 >= ' 4e+0'

$ dec2str -d0 -e2 4
1 >= ' 4e+2'

$ dec2str -d0 -e-2 4
1 >= ' 4e-2'

$ dec2str -d0 -e0 -- -108
1 >= -1.08e+2

$ dec2str -d0 -e2 -- -108
1 >= -1.08e+4

$ dec2str -d0 -e-2 -- -108
1 >= -1.08e+0

$ dec2str -d0 -e0 12345
1 >= ' 1.2345e+4'

$ dec2str -d0 -e2 12345
1 >= ' 1.2345e+6'

$ dec2str -d0 -e-2 12345
1 >= ' 1.2345e+2'

$ dec2str -d1 -e0 12345
1 >= ' 1.2345e+4'

$ dec2str -d1 -e2 12345
1 >= ' 1.2345e+6'

$ dec2str -d1 -e-2 12345
1 >= ' 1.2345e+2'

$ dec2str -d3 -e0 12345
1 >= ' 1.2345e+4'

$ dec2str -d3 -e2 12345
1 >= ' 1.2345e+6'

$ dec2str -d3 -e-2 12345
1 >= ' 1.2345e+2'

$ dec2str -d3 -e-5 12345
1 >= ' 1.2345e-1'

$ dec2str -d7 -e0 12345
1 >= ' 1.234500e+4'

$ dec2str -d7 -e2 12345
1 >= ' 1.234500e+6'

$ dec2str -d7 -e-2 12345
1 >= ' 1.234500e+2'

$ dec2str -d7 -e-5 12345
1 >= ' 1.234500e-1'

$ dec2str -d7 -e-32768 12345
1 >= ' 1.234500e-32764'

$ dec2str -d7 -e32767 12345
1 >= ' 1.234500e+32771'
