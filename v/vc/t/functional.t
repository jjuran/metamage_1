#!/usr/bin/env jtest

$ vc 'def make-repf { const f = _; { rep f } }; const r = make-repf {...}; r()'
1 >= '"{...}"'
