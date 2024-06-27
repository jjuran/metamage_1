#!/usr/bin/env jtest

$ vc struct
1 >= struct

%

$ vc 'struct {} == struct()'
1 >= true

%

$ vc 'packed struct()()'
1 >= x""

%

$ vc 'struct {x: i16} == struct {x: i16}'
1 >= true

%

$ vc 'struct {x: i16} == struct {y: i16}'
1 >= false

%

$ vc 'struct {x: i16} == struct {x: u16}'
1 >= false

%

$ vc 'struct {x: i16} [12345] == struct {y: i16} [12345]'
1 >= false

%

$ vc 'packed struct {x: i16} [12345] == packed struct {y: i16} [12345]'
1 >= true

%

$ vc 'let Point = struct {x: i16, y: i16}; (.size, .name) Point'
1 >= '(4, "")'

%

$ vc 'let Point = struct {x: i16, y: i16}; *Point'
1 >= '(("x" => i16), ("y" => i16))'

%

$ vc 'let pt = struct {x: i16, y: i16} [256, 171]; *pt'
1 >= '(("x" => 256), ("y" => 171))'

%

$ vc 'let Int = struct {x: i16}; [struct, Int, Int []] map typeof'
1 >= '[type, type, struct {"x" => i16}]'

%

$ vc 'let Int = struct {x: i16}; typeof Int [] == Int and Int [] isa Int'
1 >= true

%

$ vc 'struct Point {v: i16, h: i16}; Point'
1 >= 'struct Point {("v" => i16), ("h" => i16)}'

%

$ vc 'struct Point {v: i16, h: i16}; Point == typeof Point []'
1 >= true

%

$ vc 'struct Foo {x: i16}; struct Bar {x: i16}; Foo == Bar'
1 >= false

%

$ vc 'struct Foo {x: i16}; struct Bar {x: i16}; Foo [12345] == Bar [12345]'
1 >= false

%

$ vc 'struct Foo {x: i8}; struct Bar {x: i8}; packed Foo [9] == packed Bar [9]'
1 >= true

%

$ vc 'struct Foo {x: i16}; (.size, .name) Foo'
1 >= '(2, "Foo")'

%

$ vc 'struct Foo {x: i16}; i16.decode packed Foo [12345]'
1 >= 12345

%

$ vc 'struct Foo {x: i16, y: i16}; let foo = Foo [12, 34]; (.x, .y) foo'
1 >= '(12, 34)'

%

$ vc 'struct Foo {x: i16, y: i16}; let foo = Foo [12, 34]; *Foo, *foo'
1 >= '(("x" => i16), ("y" => i16), ("x" => 12), ("y" => 34))'

%

$ vc 'struct Foo {x: i16, y: i16}; let foo = Foo [1234]; [Foo, foo] map typeof'
1 >= '[type, struct Foo {("x" => i16), ("y" => i16)}]'

%

$ vc 'struct Point {v: i16, h: i16}; var pt = Point(); pt.h = 257; packed pt'
1 >= x"00000101"

%

$ vc 'try { big-endian struct {;} } catch { "oops" }'
1 >= '"oops"'
