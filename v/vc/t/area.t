#!/usr/bin/env jtest

$ vc 'areaof {123} == areaof [123]'
1 >= true

%

$ vc 'areaof {"x"} == areaof {"x" * 2}'
1 >= true
