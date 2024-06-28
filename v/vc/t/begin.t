#!/usr/bin/env jtest

$ vc 'begin ()'
1 >= '<iterator>'

$ vc '*begin ()'
1 >= '()'

$ vc 'begin []'
1 >= '<iterator>'

$ vc '*begin []'
1 >= '()'

$ vc 'bool begin []'
1 >= false

$ vc 'begin [0]'
1 >= '<iterator>'

$ vc '*begin [0]'
1 >= 0

$ vc 'bool begin [0]'
1 >= true

$ vc 'var p = begin [0, 1]; *p'
1 >= 0

$ vc 'var p = begin [0, 1]; ++p; *p'
1 >= 1

$ vc 'var p = begin [0, 1]; *++p'
1 >= 1

$ vc 'var p = begin [0, 1]; *p++'
1 >= 0

$ vc 'var p = begin [0, 1]; *p++; *p'
1 >= 1

$ vc 'var p = begin [0, 1]; var q = p; p++; *q'
1 >= 0

$ vc 'var p = begin [0, 1]; ++p; ++p; *p'
1 >= '()'

$ vc 'var p = begin [0, 1]; ++p; ++p; ++p; *p'
1 >= '()'

$ vc 'var p = begin []; ++p; *p'
1 >= '()'

$ vc 'var q (iterator) = begin [&q]'
1 >= '<iterator>'
