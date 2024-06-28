#!/usr/bin/env jtest

$ vc 'big-endian, little-endian, native-endian'
1 >= '(big-endian, little-endian, native-endian)'

$ vc 'typeof big-endian'
1 >= big-endian

$ vc 'typeof big-endian u32'
1 >= type

$ vc 'big-endian byte'
1 >= byte

$ vc 'big-endian u8'
1 >= u8

$ vc 'big-endian i16'
1 >= '(big-endian i16)'

$ vc 'big-endian(u16).decode x"4e75"'
1 >= 20085

$ vc 'little-endian(u16).decode x"4e75"'
1 >= 30030

$ vc 'big-endian(u16).encode 1000'
1 >= x"03e8"

$ vc 'little-endian(u16).encode 1000'
1 >= x"e803"

$ vc '{*_} big-endian(vector[u16, 2]).decode x"00010200"'
1 >= '(1, 512)'

$ vc '{*_} little-endian(vector[u16, 2]).decode x"00010200"'
1 >= '(256, 2)'

$ vc "{big-endian(mb32).encode 'v68k'}"
1 >= '{x"7636386b"}'

$ vc "{little-endian(mb32).encode 'v68k'}"
1 >= '{x"6b383676"}'

$ vc '{big-endian(mb32).decode x"776f6c66"}'
1 >= "{'wolf'}"

$ vc '{little-endian(mb32).decode x"776f6c66"}'
1 >= "{'flow'}"
