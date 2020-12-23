#!/usr/bin/env jtest

$ vc 0
1 >= 0

%

$ vc 98765432109876543210
1 >= 98765432109876543210

%

$ vc -x 0
1 >= ""

%

$ vc -x 98765432109876543210
1 >= 055aa54d38e5267eea

%

$ vc '1 + 2'
1 >= 3

%

$ vc 4-3
1 >= 1

%

$ vc 3-4
1 >= -1

%

$ vc '3 * 4'
1 >= 12

%

$ vc '42 * 98765432109876543210'
1 >= 4148148148614814814820

%

$ vc '3 + 4 * 2'
1 >= 11

%

$ vc '(3)'
1 >= 3

%

$ vc '((3))'
1 >= 3

%

$ vc '(3 + 4) * 2'
1 >= 14

%

$ vc '7 div 3'
1 >= 2

%

$ vc -- '-7 div 3'
1 >= -2

%

$ vc '7 div -3'
1 >= -2

%

$ vc -- '-7 div -3'
1 >= 2

%

$ vc '7 % 3'
1 >= 1

%

$ vc -- '-7 % 3'
1 >= -1

%

$ vc '7 % -3'
1 >= 1

%

$ vc -- '-7 % -3'
1 >= -1

%

$ vc '7 div 3'
1 >= 2

%

$ vc -- '-7 div 3'
1 >= -2

%

$ vc '7 div -3'
1 >= -2

%

$ vc -- '-7 div -3'
1 >= 2

%

$ vc '7 mod 3'
1 >= 1

%

$ vc -- '-7 mod 3'
1 >= 2

%

$ vc '7 mod -3'
1 >= -2

%

$ vc -- '-7 mod -3'
1 >= -1

%

$ vc 2^2
1 >= 4

%

$ vc -- -2^2
1 >= -4

%

$ vc 2^3
1 >= 8

%

$ vc 3^2
1 >= 9

%

$ vc 2^2^2
1 >= 16

%

$ vc 2^2^3
1 >= 256

%

$ vc 3^3^3
1 >= 7625597484987

%

$ vc 2^32-1
1 >= 4294967295

%

$ vc 2^64
1 >= 18446744073709551616

%

$ vc '9876543210987654321098765432109876543210 div 7'
1 >= 1410934744426807760156966490301410934744

%

$ vc '9876543210987654321098765432109876543210 % 7'
1 >= 2

%

$ vc true
1 >= true

%

$ vc false
1 >= false

%

$ vc '2 + 2 == 4'
1 >= true

%

$ vc '2 + 2 == 5'
1 >= false

%

$ vc '2 + 2 != 4'
1 >= false

%

$ vc '2 + 2 != 5'
1 >= true

%

$ vc 'true == true'
1 >= true

%

$ vc 'true == false'
1 >= false

%

$ vc 'false == false'
1 >= true

%

$ vc 'false == true'
1 >= false

%

$ vc 'true != true'
1 >= false

%

$ vc 'true != false'
1 >= true

%

$ vc 'false != false'
1 >= false

%

$ vc 'false != true'
1 >= true

%

$ vc '3 < 4'
1 >= true

%

$ vc '3 <= 4'
1 >= true

%

$ vc '3 > 4'
1 >= false

%

$ vc '3 >= 4'
1 >= false

%

$ vc '4 < 4'
1 >= false

%

$ vc '4 <= 4'
1 >= true

%

$ vc '4 > 4'
1 >= false

%

$ vc '4 >= 4'
1 >= true

%

$ vc '5 < 4'
1 >= false

%

$ vc '5 <= 4'
1 >= false

%

$ vc '5 > 4'
1 >= true

%

$ vc '5 >= 4'
1 >= true

%

$ vc '+false'
1 >= 0

%

$ vc -- '-false'
1 >= 0

%

$ vc '+true'
1 >= 1

%

$ vc -- '-true'
1 >= -1

%

$ vc -- 'bool 0'
1 >= false

%

$ vc -- 'bool 1'
1 >= true

%

$ vc -- 'bool 17'
1 >= true

%

$ vc -- 'bool (-1)'
1 >= true

%

$ vc -- 'bool 2^32'
1 >= true

%

$ vc -- 'bool false'
1 >= false

%

$ vc -- 'bool true'
1 >= true

%

$ vc '3;5'
1 >= 5

%

$ vc ';3'
1 >= 3

%

$ vc '3;'
1 >> .
.

%

$ vc ';'
1 >> .
.

%

$ vc ''
1 >> .
.

%

$ vc 'const x = 3; x'
1 >= 3

%

$ vc 'const x = 3'
1 >= 3

%

$ vc 'const x'
1 >= 'x'

%

$ vc 'const x; x = 3'
1 >= 3

%

$ vc 'const x; x = 3; x'
1 >= 3

%

$ vc 'var x = 3; x'
1 >= 3

%

$ vc 'var x = 3'
1 >= 3

%

$ vc 'var x; x = 3; x'
1 >= 3

%

$ vc 'var x = 3; x = 5; x'
1 >= 5

%

$ vc 'var y = const x = 3'
1 >= 3

%

$ vc 'const y = var x = 5'
1 >= 5

%

$ vc 'var x = 3; const x; x'
1 >= 3

%

$ vc 'str 0'
1 >= '"0"'

%

$ vc 'str (-43)'
1 >= '"-43"'

%

$ vc 'str 2^64'
1 >= '"18446744073709551616"'

%

$ vc 'str str 1234'
1 >= '"1234"'

%

$ vc 'str false'
1 >= '"false"'

%

$ vc 'str true'
1 >= '"true"'

%

$ vc 'hex 0'
1 >= '""'

%

$ vc 'hex 1'
1 >= '"01"'

%

$ vc 'hex (2^252 + 27742317777372353535851937790883648493)'
1 >= '"1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed"'

%

$ vc 'hex 1234'
1 >= '"04d2"'

%

$ vc 'str hex 1234'
1 >= '"04d2"'

%

$ vc 'hex str 1234'
1 >= '"31323334"'

%

$ vc 'hex hex 1234'
1 >= '"30346432"'

%

$ vc 'str 0 == str (-0)'
1 >= true

%

$ vc 'str 0 != hex 0'
1 >= true

%

$ vc 'str 64 == hex 100'
1 >= true

%

$ vc 'str 2 > str 12'
1 >= true

%

$ vc 'str 2 >= str 12'
1 >= true

%

$ vc 'str 2 < str 12'
1 >= false

%

$ vc 'str 2 <= str 12'
1 >= false

%

$ vc '()'
1 >= "()"

%

$ vc '+()'
1 >= 0

%

$ vc -- '-()'
1 >= 0

%

$ vc 'bool ()'
1 >= false

%

$ vc 'str ()'
1 >= '""'

%

$ vc '() == ()'
1 >= true

%

$ vc '() != ()'
1 >= false

%

$ vc '() == false'
1 >= false

%

$ vc '() == 0'
1 >= false

%

$ vc '() == str ()'
1 >= false

%

$ vc '1, 2'
1 >= "(1, 2)"

%

$ vc '(1, 2)'
1 >= "(1, 2)"

%

$ vc '(1, (), 2)'
1 >= "(1, 2)"

%

$ vc '(), ()'
1 >= "()"

%

$ vc '(1, 2, 3)'
1 >= "(1, 2, 3)"

%

$ vc '((1, 2), 3)'
1 >= "(1, 2, 3)"

%

$ vc '(1, (2, 3))'
1 >= "(1, 2, 3)"

%

$ vc 'bool (0, false)'
1 >= true

%

$ vc 'str (-1, true, hex 3)'
1 >= '"-1true03"'

%

$ vc 'const x = 1, 2, 3; x'
1 >= "(1, 2, 3)"

%

$ vc 'const x = 1, 2, 3; 4, x, 5'
1 >= "(4, 1, 2, 3, 5)"

%

$ vc 'var x = 1, 2, 3; var y = 4, x, 5; x = 6, 7, 8; y'
1 >= "(4, 1, 2, 3, 5)"

%

$ vc 'var x = (); x = x, 1; x = x, 2, 3; x'
1 >= "(1, 2, 3)"

%

$ vc 'const nil = (); var x = 1, 2; var y = 3, 4; var z = x, nil, y'
1 >= "(1, 2, 3, 4)"

%

$ vc 'bool str ()'
1 >= false

%

$ vc 'bool str 0'
1 >= true

%

$ vc 'bool hex 2'
1 >= true

%

$ vc '"Hello world"'
1 >= '"Hello world"'

%

$ vc 'str "string"'
1 >= '"string"'

%

$ vc 'str ("foo", "bar")'
1 >= '"foobar"'

%

$ vc 'bool "Boolean"'
1 >= true

%

$ vc 'bool ""'
1 >= false

%

$ vc 'bool "0"'
1 >= true

%

$ vc 'hex "0"'
1 >= '"30"'

%

$ vc 'hex "hex"'
1 >= '"686578"'

%

$ vc 'hex ""'
1 >= '""'

%

$ vc 'str "\x58"'
1 >= '"X"'

%

$ vc 'str "\117"'
1 >= '"O"'

%

$ vc 'hex "\377\000\a\b\f\n\r\t\v\0"'
1 >= '"ff0007080c0a0d090b00"'

%

$ vc '"foo" "bar"'
1 >= '"foobar"'

%

$ vc 'hex "1" "2"'
1 >= '"3132"'

%

$ vc '"\u0055"'
1 >= '"U"'

%

$ vc 'hex "\u2022"'
1 >= '"e280a2"'

%

$ vc 'hex "\u1f4a9"'
1 >= '"e1bd8a39"'

%

$ vc 'hex "\u{1f4a9}"'
1 >= '"f09f92a9"'

%

$ vc '(const x) = 3'
1 >= 3

%

$ vc '(const x) = 4; x'
1 >= 4

%

$ vc '(var x); (x) = 5; x'
1 >= 5

%

$ vc 'half(2)^2'
1 >= 1

%

$ vc '2^half(2)'
1 >= 2

%

$ vc '123456789^0'
1 >= 1

%

$ vc '1^234567890'
1 >= 1

%

$ vc 'str join'
1 >= '"join"'

%

$ vc 'bool join'
1 >= true

%

$ vc 'join == join'
1 >= true

%

$ vc 'hex == join'
1 >= false

%

$ vc '"hey" * 3'
1 >= '"heyheyhey"'

%

$ vc '"one" * 1'
1 >= '"one"'

%

$ vc '"zero" * 0'
1 >= '""'

%

$ vc '"" * 3'
1 >= '""'

%

$ vc '"one" * true'
1 >= '"one"'

%

$ vc '"zero" * false'
1 >= '""'

%

$ vc '"" * true'
1 >= '""'

%

$ vc '(+) ()'
1 >= 0

%

$ vc '(+) 0'
1 >= 1

%

$ vc '(+) ""'
1 >= 1

%

$ vc '(+) false'
1 >= 1

%

$ vc '(+) join'
1 >= 1

%

$ vc '(+) (1, 2)'
1 >= 2

%

$ vc '(+) (1, 2, 3, 4, 5)'
1 >= 5

%

$ vc 'const x = (); (+) x'
1 >= 0

%

$ vc 'const x = (1, 2, 3); (+) x'
1 >= 3

%

$ vc 'const x = (4, 5, 6); (+) x, x'
1 >= "(3, 4, 5, 6)"

%

$ vc '"foo" (*) 3'
1 >= '("foo", "foo", "foo")'

%

$ vc '(1, 2, 3) (*) 3'
1 >= "(1, 2, 3, 1, 2, 3, 1, 2, 3)"

%

$ vc '123 (*) 1'
1 >= 123

%

$ vc '123 (*) 0'
1 >= "()"

%

$ vc '() (*) 10^100'
1 >= "()"

%

$ vc 'var x = 1; x += 2'
1 >= 3

%

$ vc 'var x = 1; x += 2; x'
1 >= 3

%

$ vc 'var x = 1; x -= 2; x'
1 >= -1

%

$ vc 'var x = 2; x *= 3; x'
1 >= 6

%

$ vc 'var x = 7; x /:= 3; x'
1 >= 2

%

$ vc 'var x = 7; x %= 3; x'
1 >= 1

%

$ vc '"Hello".length'
1 >= 5

%

$ vc '"".length'
1 >= 0

%

$ vc '"\0".length'
1 >= 1

%

$ vc '"\xa0".length'
1 >= 1

%

$ vc '"\u{a0}".length'
1 >= 2

%

$ vc '"\u{2022}".length'
1 >= 3

%

$ vc '"\u{1f4a9}".length'
1 >= 4

%

$ vc '"three".length ^ "two".length'
1 >= 125

%

$ vc 'str().length'
1 >= 0

%

$ vc '() == ()'
1 >= true

%

$ vc '(1, 2, 3) == (1, 2, 3)'
1 >= true

%

$ vc '(1, 2, 3) == (3, 2, 1)'
1 >= false

%

$ vc '(1, 2, 3) == (1, 2)'
1 >= false

%

$ vc '(1, 2, 3) == 1'
1 >= false

%

$ vc '(1, 2, 3) == ()'
1 >= false

%

$ vc '(1, 2, 3) == (1, 3, "five")'
1 >= false

%

$ vc 'true (*) 2 == (true, true)'
1 >= true

%

$ vc 'join ""'
1 >= '""'

%

$ vc 'join "nope"'
1 >= '""'

%

$ vc 'join( "also nope", "one" )'
1 >= '"one"'

%

$ vc 'join( "", "foo", "bar" )'
1 >= '"foobar"'

%

$ vc 'join "", "foo", "bar"'
1 >= '("", "foo", "bar")'

%

$ vc 'join( " ", "foo", "bar", "baz" )'
1 >= '"foo bar baz"'

%

$ vc 'join( ", ", 1, 2, 3 )'
1 >= '"1, 2, 3"'

%

$ vc 'join( "", "x" * 1000 ) == str( "x" (*) 1000 )'
1 >= 'true'

%

$ vc '(+) (join % ())'
1 >= 1

%

$ vc '(+) (hex % "Nu")'
1 >= 1

%

$ vc '(+) (join % (" ", 0))'
1 >= 1

%

$ vc '(+) ("foo", join % "bar")'
1 >= 2

%

$ vc '(+) (join % "foo", "bar")'
1 >= 2

%

$ vc 'join % ()'
1 >= join

%

$ vc 'join % () == join'
1 >= true

%

$ vc 'hex % "Nu"'
1 >= '(hex % "Nu")'

%

$ vc '(hex % "Nu")() == hex "Nu"'
1 >= true

%

$ vc 'const csv = join % ","; csv (1, 2, 3)'
1 >= '"1,2,3"'

%

$ vc 'join % (hex % "Nu")'
1 >= '(join % (hex % "Nu"))'

%

$ vc 'join % "," % 0'
1 >= '(join % (",", 0))'

%

$ vc 'join % "," % 0 == join % (",", 0)'
1 >= true

%

$ vc '"foo", join % "bar"'
1 >= '("foo", (join % "bar"))'

%

$ vc 'join % "foo", "bar"'
1 >= '((join % "foo"), "bar")'

%

$ vc 'join % 0, join % 1, join % 2'
1 >= '((join % 0), (join % 1), (join % 2))'

%

$ vc '"glue".join'
1 >= '(join % "glue")'

%

$ vc '"glue".join()'
1 >= '""'

%

$ vc '"".join("", "len", "gth")'
1 >= '"length"'

%

$ vc '"-".join( 2015, 10, 21 )'
1 >= '"2015-10-21"'

%

$ vc '"-".join( 2015, 10, 21 ).join'
1 >= '(join % "2015-10-21")'

%

$ vc '"-".join( 2015, 10, 21 ).join (" * " (*) 2)'
1 >= '" * 2015-10-21 * "'

%

$ vc '"-".join( 2015, 10, 21 ).join % "("'
1 >= '(join % ("2015-10-21", "("))'

%

$ vc '("-".join( 2015, 10, 21 ).join % "(")(")")'
1 >= '"(2015-10-21)"'

%

$ vc '0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9'
1 >= '(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)'

%

$ vc '0xA, 0xB, 0xC, 0xD, 0xE, 0xF'
1 >= '(10, 11, 12, 13, 14, 15)'

%

$ vc '0x64, 0x100'
1 >= '(100, 256)'

%

$ vc '0x1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed'
1 >= 7237005577332262213973186563042994240857116359379907606001950938285454250989

%

$ vc '0b0, 0b1, 0b10'
1 >= '(0, 1, 2)'

%

$ vc '0b100, 0b1000, 0b10000, 0b100000, 0b1000000, 0b10000000, 0b100000000'
1 >= '(4, 8, 16, 32, 64, 128, 256)'

%

$ vc '0b1000101010000010110100000100101001001111100100101101010001001001'
1 >= 9980768741787620425

%

$ vc '0b00111011111111010010110011011101001000000111100100110110011011011001'
1 >= 69162568729436055257

%

$ vc 'unbin "0b0"'
1 >= 0

%

$ vc 'unbin "0b10"'
1 >= 2

%

$ vc 'unbin "0b1000101010000010110100000100101001001111100100101101010001001001"'
1 >= 9980768741787620425

%

$ vc 'unhex "0x0"'
1 >= 0

%

$ vc 'unhex "0x10"'
1 >= 16

%

$ vc 'unhex "0x1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed"'
1 >= 7237005577332262213973186563042994240857116359379907606001950938285454250989

%

$ vc 'unbin "0" == packed "\0"'
1 >= true

%

$ vc 'unbin "01001" == packed "\t"'
1 >= true

%

$ vc 'unbin "11110000100111111001001010101001" == packed "\u{1f4a9}"'
1 >= true

%

$ vc 'unhex "0" == packed "\0"'
1 >= true

%

$ vc 'unhex "01001" == packed "\x00\x10\x01"'
1 >= true

%

$ vc '(unhex "73776f726466697368").string'
1 >= '"swordfish"'

%

$ vc 'b"01000110011011110110111100001010".string'
1 >= '"Foo\n"'

%

$ vc 'b"00100000".string.join(1, 2, 3)'
1 >= '"1 2 3"'

%

$ vc 'x"4E75".string'
1 >= '"Nu"'

%

$ vc 'x"73776f726466697368".size'
1 >= 9

%

$ vc '()'
1 >= "()"

%

$ vc 'true'
1 >= true

%

$ vc '(12345)'
1 >= 12345

%

$ vc '"test"'
1 >= '"test"'

%

$ vc '"\007\x0a"'
1 >= '"\a\n"'

%

$ vc 'rep "\\ \"" == "\"\\\\ \\\"\""'
1 >= true

%

$ vc 'join'
1 >= join

%

$ vc '(join % ",")'
1 >= '(join % ",")'

%

$ vc '(1, 2, 3)'
1 >= "(1, 2, 3)"

%

$ vc '(join % "1" % "2", join % (hex % "3"))'
1 >= '((join % ("1", "2")), (join % (hex % "3")))'

%

$ vc 'typeof ()'
1 >= "()"

%

$ vc 'typeof true'
1 >= boolean

%

$ vc 'typeof 0'
1 >= integer

%

$ vc 'typeof ""'
1 >= string

%

$ vc 'typeof join'
1 >= function

%

$ vc 'typeof (join % ",")'
1 >= function

%

$ vc 'typeof (1, true, "thee", hex)'
1 >= '(integer, boolean, string, function)'

%

$ vc 'typeof true'
1 >= boolean

%

$ vc 'typeof typeof true'
1 >= type

%

$ vc boolean
1 >= boolean

%

$ vc function
1 >= function

%

$ vc integer
1 >= integer

%

$ vc string
1 >= string

%

$ vc type
1 >= type

%

$ vc 'var x (boolean) = true'
1 >= true

%

$ vc 'var x (integer) = 123'
1 >= 123

%

$ vc 'var x (string) = "hello"'
1 >= '"hello"'

%

$ vc 'var x (function) = join'
1 >= join

%

$ vc 'var x (function) = join % "."'
1 >= '(join % ".")'

%

$ vc 'var x (type) = string'
1 >= string

%

$ vc int
1 >= integer

%

$ vc 'bool int'
1 >= true

%

$ vc 'bool typeof ""'
1 >= true

%

$ vc 'bool typeof ()'
1 >= false

%

$ vc 'int 80'
1 >= 80

%

$ vc '(function join) % ","'
1 >= '(join % ",")'

%

$ vc 'function (join % ",")'
1 >= '(join % ",")'

%

$ vc 'function (join % ",")'
1 >= '(join % ",")'

%

$ vc 'function int'
1 >= integer

%

$ vc 'const stringify (function) = string'
1 >= string

%

$ vc 'const stringify (function) = string'
1 >= string

%

$ vc 'const raven = string % "nevermore"; raven()'
1 >= '"nevermore"'

%

$ vc 'const to_string = string'
1 >= string

%

$ vc 'var x (bool) := 1337; x'
1 >= true

%

$ vc 'var x (string) := substr, 8'
1 >= '"substr8"'

%

$ vc 'int()'
1 >= 0

%

$ vc 'var x (int) := true'
1 >= 1

%

$ vc 'u8 2^8, u16 2^16, u32 2^32, u32 2^32'
1 >= '(0, 0, 0, 0)'

%

$ vc 'i8 2^8, i16 2^16, i32 2^32, i32 2^32'
1 >= '(0, 0, 0, 0)'

%

$ vc 'u8 (-1) == 2^8 - 1'
1 >= true

%

$ vc 'u16 (-1) == 2^16 - 1'
1 >= true

%

$ vc 'u32 (-1) == 2^32 - 1'
1 >= true

%

$ vc 'u64 (-1) == 2^64 - 1'
1 >= true

%

$ vc 'i8 2^(8 - 1) == -2^(8 - 1)'
1 >= true

%

$ vc 'i16 2^(16 - 1) == -2^(16 - 1)'
1 >= true

%

$ vc 'i32 2^(32 - 1) == -2^(32 - 1)'
1 >= true

%

$ vc 'i64 2^(64 - 1) == -2^(64 - 1)'
1 >= true

%

$ vc 'i8()'
1 >= 0

%

$ vc 'var x (u8) := -1'
1 >= 255

%

$ vc 'var x (u64) := -1; x + 1 == 2^64'
1 >= true

%

$ vc 'str == string'
1 >= true

%

$ vc 'i8 == u8, u8 == bool, i32 == int'
1 >= '(false, false, false)'

%

$ vc 'int != (int, int)'
1 >= true

%

$ vc '(int, str) == (int, str), (int, str) == (str, int)'
1 >= '(true, false)'

%

$ vc 'typeof typeof == function'
1 >= true

%

$ vc '(size_t == u32) != (size_t == u64)'
1 >= true

%

$ vc '128 isa u8, 128 isa i8'
1 >= '(true, false)'

%

$ vc 'i32 isa int, i32() isa u8, i32 isa type, i32 isa function'
1 >= '(false, true, true, true)'

%

$ vc 'c_str ""'
1 >= '""'

%

$ vc 'var s (c_str) = "\001"'
1 >= '"\x01"'

%

$ vc 'typeof c_str ""'
1 >= string

%

$ vc '"\0" isa c_str, "\000foo" isa c_str, "bar\0" isa c_str, "baz0" isa c_str'
1 >= '(false, false, false, true)'

%

$ vc byte
1 >= byte

%

$ vc 'typeof byte'
1 >= type

%

$ vc 'byte(), byte 10, byte 39, byte 65, byte 92, byte 255'
1 >= "('\x00', '\n', '\'', 'A', '\\', '\xff')"

%

$ vc 'typeof byte(), int byte 128'
1 >= '(byte, 128)'

%

$ vc 'byte 256 == byte 0'
1 >= true

%

$ vc 'str (byte 65, byte 66, byte 67)'
1 >= '"ABC"'

%

$ vc 'str (byte 0xf0, byte 0x9f, byte 0x92, byte 0xa9) == "\u{1f4a9}"'
1 >= true

%

$ vc "typeof '0', int '0'"
1 >= '(byte, 48)'

%

$ vc "'A', '\n', '\\\\', '\\'', '\0', '\123', '\xFF'"
1 >= "('A', '\n', '\\', '\'', '\x00', 'S', '\xff')"

%

$ vc "bool '0', bool '\0'"
1 >= '(true, false)'

%

$ vc "'0' < '\0', 'a' < 'b'"
1 >= '(false, true)'

%

$ vc '"A"[0]'
1 >= "'A'"

%

$ vc 'var x = "12345"; x[ x.length - 1 ]'
1 >= "'5'"

%

$ vc 'mince "", mince( "", 1 )'
1 >= '("", "")'

%

$ vc 'const x = "1234567890"; mince( x, 1 ), mince x, mince( x, 3 )'
1 >= '("1234567890", "13579", "1470")'

%

$ vc '{()}, {join}, {1, 2}'
1 >= '({}, {join}, {1, 2})'

%

$ vc 'var x; {x}'
1 >= '{x}'

%

$ vc 'var x (int); {x[0]}'
1 >= '{x[0]}'

%

$ vc 'var x = 42; {x}()'
1 >= 42

%

$ vc 'var x = 42; var f = {x}; typeof f, f, f()'
1 >= '(function, {x}, 42)'

%

$ vc 'var x = 42; var f = {{x}}; f, f(), f()()'
1 >= '({{x}}, {x}, 42)'

%

$ vc 'var x = 3; var f = {x^2}; var r = f, f(); x = 4; r, f()'
1 >= '({x^2}, 9, 16)'

%

$ vc 'var x = 3; {x += 1}(); x'
1 >= 4

%

$ vc 'var id = {_}; id, id(), id("id"), id(id), id(1, 2, 3)'
1 >= '({_}, "id", {_}, 1, 2, 3)'

%

$ vc 'var twice = {_ * 2}; twice, twice("twice"), twice(12)'
1 >= '({_ * 2}, "twicetwice", 24)'

%

$ vc 'var x = 3; {x += 1; x}()'
1 >= 4

%

$ vc 'var n = 0; var one_3n = {3 * _ + 1}; var f = {n += 1; one_3n (*) _ % 2, half (*) 1 - _ % 2}; var wonder = {(wonder (*) +(_ != 1), {n} (*) +(_ == 1))(f(_)(_))}; wonder 27'
1 >= 112

%

$ vc 'var x = 3; {x += 1;}(); x'
1 >= 4

%

$ vc '{}'
1 >= '{}'

%

$ vc '{}()'
1 >= '()'

%

$ vc '{"hello"; ; ;}()'
1 >> .
.

%

$ vc '{; ; ;"hello"}()'
1 >= '"hello"'

%

$ vc '*""'
1 >= '()'

%

$ vc '*"J"'
1 >= "'J'"

%

$ vc '*"Hello"'
1 >= "('H', 'e', 'l', 'l', 'o')"

%

$ vc '(0, 1) == (false, true)'
1 >= false

%

$ vc '{0} == {false}'
1 >= false

%

$ vc '{} == {}, {;} == {;}'
1 >= '(true, true)'

%

$ vc '[]'
1 >= '[]'

%

$ vc '[()]'
1 >= '[]'

%

$ vc '[""]'
1 >= '[""]'

%

$ vc 'var x = [1, 2, 3]; typeof x, (+) x'
1 >= '((...[]), 1)'

%

$ vc 'var x = 1, 2, 3; [x]'
1 >= '[1, 2, 3]'

%

$ vc 'var x = [1, 2, 3]; [x]'
1 >= '[[1, 2, 3]]'

%

$ vc 'var x (array) = [1, 2, 3]; *x'
1 >= '(1, 2, 3)'

%

$ vc '[1, 2, 3] == ["1, 2, 3"]'
1 >= false

%

$ vc '[1, (2, 3)] == [(1, 2), 3]'
1 >= true

%

$ vc 'var x = [join % ",", {x}, {x[_]}]; x'
1 >= '[(join % ","), {x}, {x[_]}]'

%

$ vc 'var x = [join % ",", {x}, {x[_]}]; x[2] 2'
1 >= '{x[_]}'

%

$ vc 'var x = 3; var a = ++x; var b = x; var c = --x; a, b, c, x'
1 >= '(4, 4, 3, 3)'

%

$ vc 'var x (u8) = 0; var a = ++x; var b = x; var c = --x; a, b, c, x'
1 >= '(1, 1, 0, 0)'

%

$ vc 'var x = 3; var a = x++; var b = x; var c = x--; a, b, c, x'
1 >= '(3, 4, 4, 3)'

%

$ vc 'var x (u8) = 0; var a = x++; var b = x; var c = x--; a, b, c, x'
1 >= '(0, 1, 1, 0)'

%

$ vc 'var x = (); if 2 + 2 == 4 then {x = "Freedom"}; x'
1 >= '"Freedom"'

%

$ vc 'var x = (); if 2 + 2 == 5 then {x = "Slavery"}; x'
1 >= '()'

%

$ vc 'if 2 + 2 == 4 then {"Freedom"} else {"Slavery"}'
1 >= '"Freedom"'

%

$ vc 'if 2 + 2 == 5 then {"Freedom"} else {"Slavery"}'
1 >= '"Slavery"'

%

$ vc 'var items = {str( (+) _, " ", if (+) _ == 1 then{"item"} else {"items"})}; items(), items 1, items(1, 2)'
1 >= '("0 items", "1 item", "2 items")'

%

$ vc 'rep {}'
1 >= '"{}"'

%

$ vc '*[1, 2, 3]'
1 >= '(1, 2, 3)'

%

$ vc 'str( [1, 2, 3] )'
1 >= '"123"'

%

$ vc '"".lines()'
1 >= '[]'

%

$ vc '"\n".lines()'
1 >= '[""]'

%

$ vc '"foo\nbar\n".lines()'
1 >= '["foo", "bar"]'

%

$ vc '[1, 2, 3] map str'
1 >= '["1", "2", "3"]'

%

$ vc '[1, 2, 3] map str % "n = "'
1 >= '["n = 1", "n = 2", "n = 3"]'

%

$ vc '[1, 2, 3] map { _ * 2 } map str'
1 >= '["2", "4", "6"]'

%

$ vc '[1, 2, 3] map str map { v * 2 }'
1 >= '["11", "22", "33"]'

%

$ vc 'var f = {if _ isa array then {*(_ map f)} else {_}}; f ([1, [2, [3]], 4])'
1 >= '(1, 2, 3, 4)'

%

$ vc 'var x = (); var i = 0; while i < 3 do {x = x, ++i}; x'
1 >= '(1, 2, 3)'

%

$ vc 'var x = 0; var i = 0; while ++i < 1000 do {if (i % 3) * (i % 5) == 0 then {x += i}}; x'
1 >= 233168

%

$ vc '{var x; const y}'
1 >= '{(var x); (const y)}'

%

$ vc 'var x = size_t( -1 ); ++x;'
1 >> .
.

%

$ vc 'var x = size_t( -1 ); var y = x + 1; --y;'
1 >> .
.

%

$ vc 'var x = 0; while true do {x = "breaking"; break}; x'
1 >= '"breaking"'

%

$ vc 'var x = (); var i = 0; while ++i <= 3 do {while true do {x = x, i; break}}; x'
1 >= '(1, 2, 3)'

%

$ vc 'var x = (); const stop = {break}; var i = 0; while true do {x = x, i; if ++i > 3 then stop}; x'
1 >= '(0, 1, 2, 3)'

%

$ vc 'var x = 0; var i = 0; while ++i < 1000 do {if (i % 3) * (i % 5) then {continue}; x += i }; x'
1 >= 233168

%

$ vc 'const t = {[bool, int, str][_]}; var x (t 0) := 0; var y (t 1) := 0; var z (t 2) := 0; x, y, z'
1 >= '(false, 0, "0")'

%

$ vc 'var x, var y = 1, 2; [x], [y]'
1 >= '([1], [2])'

%

$ vc 'var x, var y = 1, 2'
1 >= '(1, 2)'

%

$ vc 'var x; var y; x, y = 3, 4'
1 >= '(3, 4)'

%

$ vc 'var x, var y = 1, 2; x, y = y, x'
1 >= '(2, 1)'

%

$ vc 'var x = (); const f = {var i = 0; while ++i <= _ do {x = x, i}}; f 3'
1 >= '(1, 2, 3)'

%

$ vc 'var x = (); const f = {var y; y = _; if _ > 0 then {f( y - 1 ); x = x, y};}; f( 3 ); x'
1 >= '(1, 2, 3)'

%

$ vc 'var x = (); const f = {var y = _; if _ > 0 then {f( y - 1 ); x = x, y};}; f( 3 ); x'
1 >= '(1, 2, 3)'

%

$ vc 'var x = (); const f = {var y = _; if _ > 0 then {f( _ - 1 ); x = x, y};}; f( 3 ); x'
1 >= '(1, 2, 3)'

%

$ vc 'const counter = {var x = 0; {++x}}; const f = counter(); [1 (*) 3] map f'
1 >= '[1, 2, 3]'

%

$ vc 'const f = {const x = 123}; f(); f()'
1 >= '123'

%

$ vc 'const f = {const x = 456}; if 1 then f; if 1 then f'
1 >= '456'

%

$ vc 'var i = 0; while ++i < 3 do {const x = 789}'
1 >= '789'

%

$ vc 'var x (int[]); x = []; x = [1]; x = [1, 2, 3]'
1 >= '[1, 2, 3]'

%

$ vc 'var x (int[][]); x = []; x = [[]]; x = [[], [1], [2, 3]]'
1 >= '[[], [1], [2, 3]]'

%

$ vc 'var x (...[]); x = []; x = [1, true, "3"]'
1 >= '[1, true, "3"]'

%

$ vc 'substr( "Test" )'
1 >= '"Test"'

%

$ vc 'substr( "Test", 0 )'
1 >= '"Test"'

%

$ vc 'substr( "Test", 0, 4 )'
1 >= '"Test"'

%

$ vc 'substr( "Test", 0, 4^4 )'
1 >= '"Test"'

%

$ vc 'substr( "Test", 0, (4^4)^4 - 1 )'
1 >= '"Test"'

%

$ vc 'substr( "Test", 1 )'
1 >= '"est"'

%

$ vc 'substr( "Test", 1, 2 )'
1 >= '"es"'

%

$ vc 'var x (()) = ()'
1 >= '()'

%

$ vc 'var x (str|int) = "123"'
1 >= '"123"'

%

$ vc 'var x (str|int) = 123'
1 >= '123'

%

$ vc 'var x (str|()) = "foo"'
1 >= '"foo"'

%

$ vc 'var x (str|()) = ()'
1 >= '()'

%

$ vc 'var x (u8 & i8) = 123'
1 >= '123'

%

$ vc 'substr( "Hello world", -5 )'
1 >= '"world"'

%

$ vc 'substr( "Hello world", -5, 17 )'
1 >= '"world"'

%

$ vc 'substr( "Hello world", -11, 4 )'
1 >= '"Hell"'

%

$ vc '( )'
1 >= '()'

%

$ vc '( ) == ()'
1 >= true

%

$ vc 'var x = 1; {""[(x + 2)]}'
1 >= '{""[x + 2]}'

%

$ vc '{int[()]}'
1 >= '{integer[]}'

%

$ vc '0 isa (str|int), "0" isa (str|int)'
1 >= '(true, true)'

%

$ vc '127 isa (u8 & i8), 128 isa (u8 & i8)'
1 >= '(true, false)'

%

$ vc '[] isa (int[] & str[])'
1 >= true

%

$ vc '[1, "two"] isa (int|str)[], [1, true] isa (int|str)[]'
1 >= '(true, false)'

%

$ vc '[] isa int[][], [1, 2, 3] isa int[][], [[1, 2], [3, 4]] isa int[][]'
1 >= '(true, false, true)'

%

$ vc '[].length, [""].length, [1, 2, 5].length'
1 >= '(0, 1, 3)'

%

$ vc 'if false then {"not-appearing"}'
1 >= '()'

%

$ vc '[1, 2, 3, 4, 5, 6, 7, 8, 9, 10] map {if v % 3 == 0 then {_}}'
1 >= '[3, 6, 9]'

%

$ vc 'const empty (bool) := []; empty, bool ([])'
1 >= '(false, false)'

%

$ vc 'const has_spam = {bool (_ map {if _ == "spam" then {_}})}; has_spam(["egg", "bacon"]), has_spam(["egg", "spam"])'
1 >= '(false, true)'

%

$ vc '"this is a mapping" => true'
1 >= '("this is a mapping" => true)'

%

$ vc 'typeof ("this is a mapping" => true)'
1 >= '(string => boolean)'

%

$ vc 'const m = ("this is a mapping" => 1); m isa (str => bool), m isa (str => int)'
1 >= '(false, true)'

%

$ vc 'const x (str => bool) = ("this is a type-checked mapping" => true)'
1 >= '("this is a type-checked mapping" => true)'

%

$ vc '1 => 2 => 3'
1 >= '(1 => (2 => 3))'

%

$ vc '1 => 2, 3 => 4'
1 >= '((1 => 2), (3 => 4))'

%

$ vc 'const nombres ((int => str)[]) = [1 => "one", 2 => "two", 3 => "three"]'
1 >= '[(1 => "one"), (2 => "two"), (3 => "three")]'

%

$ vc 'const nombres = [1 => "one", 2 => "two", 3 => "three"]; nombres[1]'
1 >= '(2 => "two")'

%

$ vc 'const x = 2 => "/dev/null"; [x.key], [x.value]'
1 >= '([2], ["/dev/null"])'

%

$ vc 'var x := 1; x => x, x: x, 2: 3, true: 4, (x): 5'
1 >= '((1 => 1), ("x" => 1), (2 => 3), ("true" => 4), (1 => 5))'

%

$ vc '"\u{1D563 1D566 1D553 1D56A}" == "\u{1D563}\u{1D566}\u{1D553}\u{1D56A}"'
1 >= true

%

$ vc '"\u{65 73 63 61 70 65 20 73 65 71 75 65 6E 63 65 73}"'
1 >= '"escape sequences"'

%

$ vc '"" % (), "Hello" % (), "100%%" % ()'
1 >= '("", "Hello", "100%")'

%

$ vc '"Hello %s" % "world", "%s%s%s" % ("foo", "", "bar")'
1 >= '("Hello world", "foobar")'

%

$ vc '"I hate to be the %s, but you would be %s..." % (-1, -2)'
1 >= '"I hate to be the -1, but you would be -2..."'

%

$ vc 'var x = 5; const y = do {x -= 2; "blue"}; x, y'
1 >= '(3, "blue")'

%

$ vc 'var x; {do {x = _}}( "Greetings" ); x'
1 >= '"Greetings"'

%

$ vc 'var x = "break"; while true do {do {break}; x = "not reached"; break}; x'
1 >= '"break"'

%

$ vc 'var x = "break"; while true do {do {break} while false; x = "not broken"; break}; x'
1 >= '"not broken"'

%

$ vc 'var x = (); var i = 0; do {x = x, ++i} while i < 3; x'
1 >= '(1, 2, 3)'

%

$ vc 'assert true; assert 2 + 2 == 4'
1 >> .
.

%

$ vc 'assert (1 == 2) == (3 == 4)'
1 >> .
.

%

$ vc 'try {throw "foo"} catch {"bar"}'
1 >= '"bar"'

%

$ vc 'const f = {throw "foo"}; try {f()} catch {"baz"}'
1 >= '"baz"'

%

$ vc 'try {throw "foo"} catch {str(_, "bar")}'
1 >= '"foobar"'

%

$ vc '(abs -42), (rep [])'
1 >= '(42, "[]")'

%

$ vc '(rep [].length), (half (4)^2)'
1 >= '("0", 8)'

%

$ vc 'str("test")[0], [str, int][1]()'
1 >= "('t', 0)"

%

$ vc '"" == "", "" < ".", "\x01" < "\xFF", "\xFF" > "\x01", "\xFF" < "\xFF\x01"'
1 >= '(true, true, true, true, true)'

%

$ vc 'const zeros = "0" * 2^8 * 2^8; "0" < zeros, "1" > zeros'
1 >= '(true, true)'

%

$ vc 'packed(), packed "Nu", packed byte 7'
1 >= '(x"", x"4e75", x"07")'

%

$ vc 'packed() == packed "", packed "" < packed "\0"'
1 >= '(true, true)'

%

$ vc 'packed "\x01" < packed "\xFF", "\xFF" < "\xFF\x01"'
1 >= '(true, true)'

%

$ vc 'const x = packed "Hi\0"; *x, x[0]'
1 >= "('H', 'i', '\x00', 'H')"

%

$ vc 'const x = "Hi\x00 \u{1f4a9}"; x == str packed x, x == packed( x ).string'
1 >= "(false, true)"

%

$ vc 'const x = "Hello\x00 \u{1f4a9}"; x == str *(packed x), (+) *(packed x)'
1 >= "(true, 11)"

%

$ vc 'str *packed( "foo", *"bar", packed "baz" )'
1 >= '"foobarbaz"'

%

$ vc 'packed().size, packed("\u{1f4a9}\0").size'
1 >= '(0, 5)'

%

$ vc 'sha256 ""'
1 >= 'x"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"'

%

$ vc 'sha256 sha256 ""'
1 >= 'x"5df6e0e2761359d30a8275058e299fcc0381534545f55cf43e41983f5d4c9456"'

%

$ vc 'bool packed(), bool packed byte()'
1 >= '(false, true)'

%

$ vc 'int "010"'
1 >= '10'

%

$ vc 'int "-010"'
1 >= '-10'

%

$ vc 'int "-0"'
1 >= '0'

%

$ vc 'try {int "foo"} catch {"not"}'
1 >= '"not"'

%

$ vc 'try {int "-"} catch {"not"}'
1 >= '"not"'

%

$ vc 'try {int ""} catch {"not"}'
1 >= '"not"'

%

$ vc 'int "+123"'
1 >= '123'

%

$ vc 'const x = "foo"; x "bar"'
1 >= '"foobar"'

%

$ vc 'const x = "foo"; x x, x 2, x true, x "U"[0], x()'
1 >= '("foofoo", "foo2", "footrue", "fooU", "foo")'

%

$ vc 'const x = "foo"; x [0], x "B"[0]'
1 >= '("foo0", "fooB")'

%

$ vc 'const x = "foo"; x[0], x"B"[0]'
1 >= "('f', '\v')"

%

$ vc 'null, str null, rep null'
1 >= '(null, "null", "null")'

%

$ vc '(+) null, bool null, null null, null()'
1 >= '(1, false, null, null)'

%

$ vc 'null isa type, null isa null, null isa (), () isa null'
1 >= '(true, true, false, false)'

%

$ vc 'rep if ... then {...}'
1 >= '"..."'

%

$ vc 'const f = lambda {return "return"; "leave"}; f()'
1 >= '"return"'

%

$ vc 'const f = lambda {if true then {return 1}; return 2; 3 }; f()'
1 >= '1'

%

$ vc 'const f = lambda {while true do {return 1; break;}; return 2; 3 }; f()'
1 >= '1'

%

$ vc 'const f = lambda {_}; f("round")'
1 >= '"round"'

%

$ vc 'const f = lambda {return _;}; f("trip")'
1 >= '"trip"'

%

$ vc 'const f = lambda {}; f, bool f, f isa function'
1 >= '((lambda {}), true, true)'

%

$ vc '[1, 2, 3, 4, 5][[4]]'
1 >= '[5]'

%

$ vc '[1, 2, 3, 4, 5][[2, 0, 3, 0]]'
1 >= '[3, 1, 4, 1]'

%

$ vc '"Hello world"[[4]]'
1 >= '"o"'

%

$ vc '"Hello world"[[3, 1, 6, 10]]'
1 >= '"lewd"'

%

$ vc 'x"FACEDEAD"[[3]]'
1 >= 'x"ad"'

%

$ vc 'x"FACEDEAD"[[0, 2]]'
1 >= 'x"fade"'

%

$ vc '[][[]], ""[[]], x""[[]]'
1 >= '([], "", x"")'

%

$ vc '1 .. 5, 1 -> 5'
1 >= '((1 .. 5), (1 -> 5))'

%

$ vc '*(1..5)'
1 >= '(1, 2, 3, 4, 5)'

%

$ vc '1..3 == 1..3'
1 >= 'true'

%

$ vc '*(1..3) == *(1->4)'
1 >= 'true'

%

$ vc '*(1..1)'
1 >= '1'

%

$ vc '*(1..0)'
1 >= '()'

%

$ vc '*(0->1)'
1 >= '0'

%

$ vc '*(1->1)'
1 >= '()'

%

$ vc '1..1, 1..0, 0->0'
1 >= '((1 .. 1), (1 .. 0), (0 -> 0))'

%

$ vc '1..0 == 2..1, 0->0 == 1->1'
1 >= '(false, false)'

%

$ vc 'const az = "_abcdefghijklmnopqrstuvwxyz"; az[ 14 .. 16 ], az[ 24 -> 27 ]'
1 >= '("nop", "xyz")'

%

$ vc -- '-"", -"a", -x"", -x"a"'
1 >= '("", "a", x"", x"0a")'

%

$ vc -- '-"foobarbaz", -x"4afc"'
1 >= '("zabraboof", x"fc4a")'

%

$ vc -- '-[], -[1], -(0 -> 0), -(1 .. 1)'
1 >= '([], [1], [], [1])'

%

$ vc -- '-[1, 2, 3, 4, 5], -(0 -> 8), -(1 .. 3)'
1 >= '([5, 4, 3, 2, 1], [7, 6, 5, 4, 3, 2, 1, 0], [3, 2, 1])'

%

$ vc 'var x = []; x <-- (); x'
1 >= '[]'

%

$ vc 'var x = [1, 2]; x <-- (); x'
1 >= '[1, 2]'

%

$ vc 'var x = []; x <-- 3, 4; x'
1 >= '[3, 4]'

%

$ vc 'var x = [1, 2]; x <-- 3, 4; x'
1 >= '[1, 2, 3, 4]'

%

$ vc 'def len { _.length }; len "Hello"'
1 >= '5'

%

$ vc 'def quine { const qq = "\""; return qq _ qq " " _; }; quine "is quoted"'
1 >= '"\"is quoted\" is quoted"'

%

$ vc '[1, 2,], {3, 4,}, (5, 6,)'
1 >= '([1, 2], {3, 4}, 5, 6)'

%

$ vc 'try { " ".join( 1, {2}) } catch {"nope"}'
1 >= '"nope"'

%

$ vc '3 <=> 4, 4 <=> 3, 2 + 2 <=> 4'
1 >= '(-1, 1, 0)'

%

$ vc -- '-1 .. 1 map { const x = v; -1 .. 1 map { x <=> v } }'
1 >= '[[0, -1, -1], [1, 0, -1], [1, 1, 0]]'

%

$ vc '"foo" <=> "bar", "Foo" <=> "bar", "foo" "bar" <=> "foobar"'
1 >= '(1, -1, 0)'

%

$ vc 'var x = 0; var y = 1; var z = 2; {x > y <=> z}'
1 >= '{x > (y <=> z)}'

%

$ vc 'not true, not false'
1 >= '(false, true)'

%

$ vc 'not not true, not not false'
1 >= '(true, false)'

%

$ vc 'def ok {str ("not " (*) not _, "ok")}; ok 1, ok ""'
1 >= '("ok", "not ok")'

%

$ vc '[null, 0, "", [], false, true, "0", [[]]] map {_ and ...}'
1 >= '[null, 0, "", [], false, ..., ..., ...]'

%

$ vc '[null, 0, "", [], false, true, "0", [[]]] map {v or ...}'
1 >= '[..., ..., ..., ..., ..., true, "0", [[]]]'

%

$ vc 'var a = 1; var b = 2; var c = 3; {a and b or c, a or b and c}'
1 >= '{((a and b) or c), (a or (b and c))}'

%

$ vc 'null: null, 0: [1, 2, 3]'
1 >= '(("null" => null), (0 => [1, 2, 3]))'

%

$ vc '[[null], [], [1], [1, 2, 3]] map {try {value: *_} catch {"nope"}}'
1 >= '[("value" => null), "nope", ("value" => 1), "nope"]'

%

$ vc '[[null], [], [1], [1, 2, 3]] map {try {*_ => "key"} catch {"nope"}}'
1 >= '["nope", "nope", (1 => "key"), "nope"]'

%

$ vc 'byte^[], bool str^[], typeof packed^[], typeof bool^..., int^... | ()'
1 >= '((byte^[]), false, (packed^...), type, ((integer^...) | ()))'

%

$ vc 'str^[foo: 1, bar: "none", baz: null]'
1 >= '(string^[("foo" => 1), ("bar" => "none"), ("baz" => null)])'

%

$ vc 'const x = str^[foo: 1, bar: "none", baz: null]; x.length, bool x'
1 >= '(3, true)'

%

$ vc 'const x = str^[foo: 1, bar: "none", baz: null]; x.keys'
1 >= '["foo", "bar", "baz"]'

%

$ vc 'const x = str^[foo: 1, bar: "none", baz: null]; x.values'
1 >= '[1, "none", null]'

%

$ vc 'const x = str^[foo: 1, bar: "none", baz: null]; *x'
1 >= '(("foo" => 1), ("bar" => "none"), ("baz" => null))'

%

$ vc 'const x = str^[foo: 1, bar: "none", baz: null]; -x'
1 >= '(string^[("baz" => null), ("bar" => "none"), ("foo" => 1)])'

%

$ vc 'const x = str^[foo: 1, bar: "none", baz: null]; x["bar"], x["foo"]'
1 >= '("none", 1)'

%

$ vc 'const x = str^[foo: 1, bar: "none", baz: null]; x[["baz", "foo"]], x[[]]'
1 >= '([null, 1], [])'

%

$ vc 'str^[] isa str^[], str^[] isa int^[], str^[foo: 1, bar: 2] isa str^[]'
1 >= '(true, false, true)'

%

$ vc '0 .. 5 map { _^2 }'
1 >= '[0, 1, 4, 9, 16, 25]'

%

$ vc '0 .. 0 map { _^2 }, 0 -> 0 map { _^2 }'
1 >= '([0], [])'

%

$ vc -- '-3 .. 1 map { v^2 }, -2 -> -1 map { v^2 }'
1 >= '([9, 4, 1, 0, 1], [4])'

%

$ vc '"spam" in ""'
1 >= 'false'

%

$ vc '"spam" in "SPAM"'
1 >= 'false'

%

$ vc '"spam" in "spam"'
1 >= 'true'

%

$ vc '"spam" in "spam\0"'
1 >= 'true'

%

$ vc '"spam" in "My hovercraft is full of spam"'
1 >= 'true'

%

$ vc '"spam" in x"7370616d"'
1 >= 'true'

%

$ vc 'not "spam" in -"maps"'
1 >= 'false'

%

$ vc 'const x = 1 .. 3; 3 in x, 4 in x, 2 in x, 5 in x, 1 in x, 0 in x'
1 >= '(true, false, true, false, true, false)'

%

$ vc 'const x = 0 -> 2^64; -1 in x, 0 in x, 2^64 - 1 in x, 2^64 in x'
1 >= '(false, true, true, false)'

%

$ vc '"spam" in ["egg", "bacon"]'
1 >= 'false'

%

$ vc '"spam" in ["egg", "sausage", "bacon"]'
1 >= 'false'

%

$ vc '"spam" in ["egg", "spam"]'
1 >= 'true'

%

$ vc '"spam" in ["egg", "sausage", "spam"]'
1 >= 'true'

%

$ vc '"spam" in ["bacon", "egg", "sausage", "spam"]'
1 >= 'true'

%

$ vc '"spam" in ["spam", "egg", "sausage", "spam"]'
1 >= 'true'

%

$ vc '"spam" in ["spam" (*) 6, "baked beans", "spam" (*) 4]'
1 >= 'true'

%

$ vc '"spam" in ["spammy"]'
1 >= 'false'

%

$ vc '"spam" in ["spam\0"]'
1 >= 'false'

%

$ vc '"spam" in ["spam"]'
1 >= 'true'

%

$ vc '"spam" in ["bloody Vikings"]'
1 >= 'false'

%

$ vc '"spam" in []'
1 >= 'false'

%

$ vc 'const x = [1: 2, 3: 4]; (1: 1) in x, (2: 2) in x, (1: 2) in x'
1 >= '(false, false, true)'

%

$ vc 'const x = [1: 2, 3: 4]; try { 1 in x } catch { "nope" }'
1 >= '"nope"'

%

$ vc 'const x = int^[1: 2, 3: 4]; 1 in x, 2 in x'
1 >= '(true, false)'

%

$ vc 'const x = int^[1: 2, 3: 4]; try { (1: 2) in x } catch { "nope" }'
1 >= '"nope"'

%

$ vc 'const x = int^[1: 2, 3: 4]; try { "1" in x } catch { "nope" }'
1 >= '"nope"'

%

$ vc '"spam" in str^[]'
1 >= 'false'

%

$ vc 'str^[foo: 1, bar: 2] map {v.value => v.key}'
1 >= '[(1 => "foo"), (2 => "bar")]'

%

$ vc 'var x = [1, 2, 5]; x[ 2 ] = 3; x'
1 >= '[1, 2, 3]'

%

$ vc 'var x = [1, 2, 5]; x[ x[1] ] = 3; x'
1 >= '[1, 2, 3]'

%

$ vc 'var x = [1, 2, 5]; x[ 2 ] = x; x'
1 >= '[1, 2, [1, 2, 5]]'

%

$ vc 'var x = str^[foo: 1, bar: 2]; x[ "bar" ] = null; x'
1 >= '(string^[("foo" => 1), ("bar" => null)])'

%

$ vc 'var x = str^[foo: 1, bar: 2]; x[ "baz" ] = null; x'
1 >= '(string^[("foo" => 1), ("bar" => 2), ("baz" => null)])'

%

$ vc 'var x = str^[foo: 1, bar: 2]; x[ "foo" ] = x; x'
1 >= '(string^[("foo" => (string^[("foo" => 1), ("bar" => 2)])), ("bar" => 2)])'

%

$ vc 'var a = []; for i in 1 .. 3 do { a <-- i }; a'
1 >= '[1, 2, 3]'

%

$ vc 'var a = []; for i in -(1 .. 5) do { a <-- i }; a'
1 >= '[5, 4, 3, 2, 1]'

%

$ vc 'var a = []; for i in 1 -> 1 do { a <-- i }; a'
1 >= '[]'

%

$ vc 'var a = []; for x in ["foo", "bar", "baz"] do { a <-- x }; a'
1 >= '["foo", "bar", "baz"]'

%

$ vc 'var a = []; for x in [] do { a <-- x }; a'
1 >= '[]'

%

$ vc 'var a = []; for x in str^[foo: 1, bar: 2] do { a <-- x }; a'
1 >= '[("foo" => 1), ("bar" => 2)]'

%

$ vc 'var x = [1, 2, 3]; var y = x; x <-- 4; x, y'
1 >= '([1, 2, 3, 4], [1, 2, 3])'

%

$ vc '"bytes" map {_}'
1 >= "['b', 'y', 't', 'e', 's']"

%

$ vc 'x"4e75" map {v}'
1 >= "['N', 'u']"

%

$ vc '"" map {_}'
1 >= '[]'

%

$ vc '*str^[]'
1 >= '()'

%

$ vc 'if 1 then {2} else if 3 then {4} else {5}'
1 >= 2

%

$ vc 'if 0 then {2} else if 3 then {4} else {5}'
1 >= 4

%

$ vc 'if 0 then {2} else if 0 then {4} else {5}'
1 >= 5

%

$ vc 'var x = 3; var y = 4; x <-> y; x, y'
1 >= '(4, 3)'

%

$ vc 'var x; var y = "moved"; x <- y; x'
1 >= '"moved"'

%

$ vc 'var x = 0; var y = 1; var z = 2; x <- y <- z; x, y'
1 >= '(1, 2)'

%

$ vc 'var x = 0; var y = 1; var z = 2; x <- y <- z = 3; x, y, z'
1 >= '(1, 2, 3)'

%

$ vc 'x"1234" * 3'
1 >= 'x"123412341234"'

%

$ vc 'x"1234" * true'
1 >= 'x"1234"'

%

$ vc 'x"1234" * false'
1 >= 'x""'

%

$ vc 'x"1234" x"abcd"'
1 >= 'x"1234abcd"'

%

$ vc 'var x = [1, 2, 3]; ++x[1]; x'
1 >= '[1, 3, 3]'

%

$ vc 'typeof int[]'
1 >= type

%

$ vc 'var x (u8) = 123; try { x = -1 } catch {}; x'
1 >= 123

%

$ vc 'var x (u8) = 123; var y = -1; try { x <- y } catch {}; x'
1 >= 123

%

$ vc 'var x (u8) = 123; var y = -1; try { x <-> y } catch {}; x'
1 >= 123

%

$ vc 'var x (u8[] | i8[]); x = [1, 2]; x[0] = -1; x'
1 >= '[-1, 2]'

%

$ vc 'var x (u8[] | str[]); x = [1,2]; var y = ""; try { x[0] = y } catch {x}'
1 >= '[1, 2]'

%

$ vc 'var x (u8[] | str[]); x = [1,2]; var y = ""; try { x[0] <- y } catch {x}'
1 >= '[1, 2]'

%

$ vc 'var x (u8[] | str[]); x = [1,2]; var y = ""; try { x[0] <-> y } catch {x}'
1 >= '[1, 2]'

%

$ vc 'var x = []; try { x[ 0 ] = "" } catch { "nope" }'
1 >= '"nope"'

%

$ vc 'var x = [1, 2, 3]; try { x[ 4 ] = 5 } catch { "right out" }'
1 >= '"right out"'

%

$ vc 'const UTF = 8; UTF - 8: 0, UTF-8: 1'
1 >= '((0 => 0), ("UTF-8" => 1))'

%

$ vc "'a' .. 'f'"
1 >= "('a' .. 'f')"

%

$ vc "*('a' .. 'f')"
1 >= "('a', 'b', 'c', 'd', 'e', 'f')"

%

$ vc "'a' .. 'f' map {_}"
1 >= "['a', 'b', 'c', 'd', 'e', 'f']"

%

$ vc "str *('a' .. 'f')"
1 >= '"abcdef"'

%

$ vc 'var s = "8675309"; translate( &s, "0123456789", "1234567890" ); s'
1 >= '"9786410"'

%

$ vc '[] == []'
1 >= true

%

$ vc 'var x (str^...) = str^[]'
1 >= '(string^[])'

%

$ vc 'x"4e75".string "?"'
1 >= '"Nu?"'

%

$ vc '"head: " "glue".join( "(", "/", ")" )'
1 >= '"head: (glue/glue)"'

%

$ vc '{_ / _, _ div _}'
1 >= '{(_ / _), (_ div _)}'

%

$ vc '"foo" byte(32) "bar"'
1 >= '"foo bar"'

%

$ vc 'byte(5) packed "Hello"'
1 >= 'x"0548656c6c6f"'

%

$ vc 'var x = "\xe2\x80\xa2"; translate( &x, "\xe2\x80\xa2", "abc" ); x'
1 >= '"abc"'
