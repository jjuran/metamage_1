#!/usr/bin/env jtest

$ vc 'mb32()'
1 >= "'\x00\x00\x00\x00'"

%

$ vc 'mb32 x"12345678"'
1 >= "'\x124Vx'"

%

$ vc "'TEXT'"
1 >= "'TEXT'"

%

$ vc "'STR ', 'ICN#'"
1 >= "('STR ', 'ICN#')"

%

$ vc "'fnd\xA7'"
1 >= "'fnd\xa7'"

%

$ vc "packed 'fnd\xA7'"
1 >= 'x"666e64a7"'

%

$ vc "mb32() == '\x00\x00\x00\x00'"
1 >= true

%

$ vc 'str mb32 packed "fnd\xA7" == "fnd\xc3\x9f"'
1 >= true

%

$ vc 'mb32 "foo\u2022"'
1 >= "'foo\xa5'"

%

$ vc "bool mb32(), bool '\x00\x00\x00\x01', bool '\xFF\xFF\xFF\xFF'"
1 >= '(false, true, true)'

%

$ vc 'const code (mb32) := packed "\u{1f4a9}"'
1 >= "'\xf0\x9f\x92\xa9'"

%

$ vc "const table = mb32^[ 'sysa': 2, 'sysv': 0x922 ]; table[ 'sysv' ] >= 0x904"
1 >= true

%

$ vc "(mb32^[ 'sys1': 10, 'sys2': 4, 'sys3': 11 ])[[ 'sys1', 'sys2' ]]"
1 >= "[10, 4]"

%

$ vc 'mb32() isa mb32, typeof mb32(), typeof mb32'
1 >= '(true, mb32, type)'

%

$ vc "'1234' isa byte, '1' isa mb32"
1 >= '(false, false)'

%

$ vc '{const code (mb32) := "1234"}'
1 >= "{'1234'}"

%

$ vc "'1234' <=> '4321'"
1 >= -1
