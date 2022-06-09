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
