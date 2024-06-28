#!/usr/bin/env jtest

$ vc 'record'
1 >= record

$ vc 'typeof record'
1 >= type

$ vc 'record (foo: 1)'
1 >= '(record ("foo" => 1))'

$ vc 'record (foo: 1, bar: null)'
1 >= '(record (("foo" => 1), ("bar" => null)))'

$ vc '*record(foo: 1, bar: null)'
1 >= '(("foo" => 1), ("bar" => null))'

$ vc 'typeof record (foo: 1, bar: null)'
1 >= record

$ vc 'record (foo: 1, bar: null) isa record'
1 >= true

$ vc 'const a = [foo: 1, bar: null]; a isa record, str^a isa record'
1 >= '(false, false)'

$ vc '*record(foo: 1, bar: null) isa record'
1 >= false

$ vc 'const r = record (foo: 1, bar: null); r.foo'
1 >= 1

$ vc 'record(foo: 1, bar: "baz").bar'
1 >= '"baz"'

$ vc 'record(foo: 1) == record(foo: 1)'
1 >= true

$ vc 'record(foo: 1) == record(foo: 2)'
1 >= false

$ vc 'var r (record) = record(foo: &r)'
1 >= '(record ("foo" => (&r)))'
