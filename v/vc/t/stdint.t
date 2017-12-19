#!/usr/bin/env jtest

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map bool'
1 >= '[true, true, true, true, true, true, true, true]'

%

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map {typeof _}'
1 >= '[type, type, type, type, type, type, type, type]'

%

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map {_ isa type}'
1 >= '[true, true, true, true, true, true, true, true]'

%

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map { _() }'
1 >= '[0, 0, 0, 0, 0, 0, 0, 0]'

%

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map { _ .-1 }'
1 >= '[255, -1, 65535, -1, 4294967295, -1, 18446744073709551615, -1]'

%

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map { _ 10^10 }'
1 >= '[0, 0, 58368, -7168, 1410065408, 1410065408, 10000000000, 10000000000]'
