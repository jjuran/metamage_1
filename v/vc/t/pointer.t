#!/usr/bin/env jtest

$ vc 'each "Hello"'
1 >= '<pointer>'

%

$ vc '*each "Hello"'
1 >= "'H'"

%

$ vc 'bool each "Hello"'
1 >= true

%

$ vc 'each ""'
1 >= '<pointer>'

%

$ vc 'bool each ""'
1 >= false

%

$ vc 'var p = each "Hello"; *p'
1 >= "'H'"

%

$ vc 'var p = each "Hello"; ++p; *p'
1 >= "'e'"

%

$ vc 'var p = each "Hello"; ++p; ++p; ++p; ++p; *p'
1 >= "'o'"

%

$ vc 'var p = each "Hello"; ++p; ++p; ++p; ++p; *++p'
1 >= '()'

%

$ vc '*each x"4e75"'
1 >= "'N'"
