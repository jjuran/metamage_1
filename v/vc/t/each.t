#!/usr/bin/env jtest

$ vc 'each ()'
1 >= '<iterator>'

%

$ vc '*each ()'
1 >= '()'

%

$ vc 'each []'
1 >= '<iterator>'

%

$ vc '*each []'
1 >= '()'

%

$ vc 'bool each []'
1 >= false

%

$ vc 'each [0]'
1 >= '<iterator>'

%

$ vc '*each [0]'
1 >= 0

%

$ vc 'bool each [0]'
1 >= true

%

$ vc 'var p = each [0, 1]; *p'
1 >= 0

%

$ vc 'var p = each [0, 1]; ++p; *p'
1 >= 1

%

$ vc 'var p = each [0, 1]; *++p'
1 >= 1

%

$ vc 'var p = each [0, 1]; *p++'
1 >= 0

%

$ vc 'var p = each [0, 1]; *p++; *p'
1 >= 1

%

$ vc 'var p = each [0, 1]; var q = p; p++; *q'
1 >= 0

%

$ vc 'var p = each [0, 1]; ++p; ++p; *p'
1 >= '()'

%

$ vc 'var p = each [0, 1]; ++p; ++p; ++p; *p'
1 >= '()'

%

$ vc 'var p = each []; ++p; *p'
1 >= '()'
