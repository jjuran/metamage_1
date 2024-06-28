#!/usr/bin/env jtest

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map bool'
1 >= '[true, true, true, true, true, true, true, true]'

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map {typeof _}'
1 >= '[type, type, type, type, type, type, type, type]'

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map {_ isa type}'
1 >= '[true, true, true, true, true, true, true, true]'

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map { _() }'
1 >= '[0, 0, 0, 0, 0, 0, 0, 0]'

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map { _ -1 }'
1 >= '[255, -1, 65535, -1, 4294967295, -1, 18446744073709551615, -1]'

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map { _ 10^10 }'
1 >= '[0, 0, 58368, -7168, 1410065408, 1410065408, 10000000000, 10000000000]'

$ vc '[u8, i8, u16, i16, u32, i32, u64, i64] map {_.size}'
1 >= '[1, 1, 2, 2, 4, 4, 8, 8]'

$ vc '[0, 42, 127, 128, 255] map u8.encode'
1 >= '[x"00", x"2a", x"7f", x"80", x"ff"]'

$ vc '[0, 42, 127, -128, -1] map i8.encode'
1 >= '[x"00", x"2a", x"7f", x"80", x"ff"]'

$ vc '[x"00", x"2a", x"7f", x"80", x"ff"] map u8.decode'
1 >= '[0, 42, 127, 128, 255]'

$ vc '[x"00", x"2a", x"7f", x"80", x"ff"] map i8.decode'
1 >= '[0, 42, 127, -128, -1]'

$ vc '[u16, i16, u32, i32] map { _.decode -(_.encode 1) }'
1 >= '[256, 256, 16777216, 16777216]'

$ vc '[u16, i16, u32, i32] map { _.decode -(_.encode 255) }'
1 >= '[65280, -256, 4278190080, -16777216]'
