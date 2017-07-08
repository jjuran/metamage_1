#!/usr/bin/env jtest

$ vc 'def make-repf { const f = _; { rep f } }; const r = make-repf {...}; r()'
1 >= '"{...}"'

%

$ vc 'def mr { const f = _; { rep f } }; [mr {}, mr {123}, mr {}][1]()'
1 >= '"{123}"'
