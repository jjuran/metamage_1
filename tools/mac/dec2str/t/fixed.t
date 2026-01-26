#!/usr/bin/env jtest

$ dec2str -d0 -x0 0
1 >= 0

$ dec2str -d0 -x0 -- -0
1 >= -0

$ dec2str -d0 -x0 I
1 >= INF

$ dec2str -d0 -x0 -- -I
1 >= -INF

$ dec2str -d0 -x0 N
1 >= 'NAN(000)'

$ dec2str -d0 -x0 -- -N
1 >= '-NAN(000)'

$ dec2str -d3 -x0 0
1 >= 0.000

$ dec2str -d3 -x2 0
1 >= 0.000

$ dec2str -d3 -x-2 0
1 >= 0.000

$ dec2str -d0 -x0 4
1 >= 4

$ dec2str -d0 -x2 4
1 >= 400

$ dec2str -d0 -x-2 4
1 >= 0.04

$ dec2str -d0 -x0 -- -108
1 >= -108

$ dec2str -d0 -x2 -- -108
1 >= -10800

$ dec2str -d0 -x-2 -- -108
1 >= -1.08

$ dec2str -d0 -x0 12345
1 >= 12345

$ dec2str -d0 -x2 12345
1 >= 1234500

$ dec2str -d0 -x-2 12345
1 >= 123.45

$ dec2str -d1 -x0 12345
1 >= 12345.0

$ dec2str -d1 -x2 12345
1 >= 1234500.0

$ dec2str -d1 -x-2 12345
1 >= 123.45

$ dec2str -d3 -x0 12345
1 >= 12345.000

$ dec2str -d3 -x2 12345
1 >= 1234500.000

$ dec2str -d3 -x-2 12345
1 >= 123.450

$ dec2str -d3 -x-5 12345
1 >= 0.12345
