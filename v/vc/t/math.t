#!/usr/bin/env jtest

$ vc 'abs 0'
1 >= 0

%

$ vc 'abs 8'
1 >= 8

%

$ vc 'abs (-8)'
1 >= 8

%

$ vc 'half 0'
1 >= 0

%

$ vc 'half 8'
1 >= 4

%

$ vc 'half 7'
1 >= 3

%

$ vc 'half (-8)'
1 >= -4

%

$ vc 'half (-7)'
1 >= -3

%

$ vc 'half 2^100 == 2^99'
1 >= true

%

$ vc 'abs == Math.abs'
1 >= true

%

$ vc 'half == Math.half'
1 >= true
