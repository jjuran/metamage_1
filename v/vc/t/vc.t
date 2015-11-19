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

$ vc 1+2
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

$ vc '7 / 3'
1 >= 2

%

$ vc -- '-7 / 3'
1 >= -2

%

$ vc '7 / -3'
1 >= -2

%

$ vc -- '-7 / -3'
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

$ vc '9876543210987654321098765432109876543210 / 7'
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

$ vc 'abs 0'
1 >= 0

%

$ vc 'abs 8'
1 >= 8

%

$ vc 'abs (-8)'
1 >= 8

%

$ vc 'half 0'
1 >= 0

%

$ vc 'half 8'
1 >= 4

%

$ vc 'half 7'
1 >= 3

%

$ vc 'half (-8)'
1 >= -4

%

$ vc 'half (-7)'
1 >= -3

%

$ vc 'half 2^100 == 2^99'
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

$ vc PC
1 >= 1

%

$ vc -- -PC
1 >= -1

%

$ vc ';' PC
1 >= 2

%

$ vc 'PC = 1; PC'
1 >= 1

%

$ vc 'PC = 3; PC' 1 2
1 >= 3

%

$ vc 'PC = 3; PC'
1 >= 3

%

$ vc 'PC = 3; PC' 1/0 undef
1 >= 3

%

$ vc PC=3 1/0 undef 'PC = 5; PC'
1 >= 5

%

$ vc 'const x = 3;' x
1 >= 3

%

$ vc 'const x = 3'
1 >= 3

%

$ vc 'const x' 'x = 3' x
1 >= 3

%

$ vc 'var x = 3;' x
1 >= 3

%

$ vc 'var x = 3'
1 >= 3

%

$ vc 'var x' 'x = 3' x
1 >= 3

%

$ vc 'var x = 3;' 'x = 5' x
1 >= 5

%

$ vc 'var x = 0; var i = 1;' 'x = x + i * +((i % 3) * (i % 5) == 0); i = i + 1; PC = PC - +(i < 1000)' x
1 >= 233168

%

$ vc 'var y = const x = 3'
1 >= 3

%

$ vc 'const y = var x = 5'
1 >= 5

%

$ vc 'var x = 3;' 'const x' x
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

$ vc 'print "\n"'
1 >> .


.

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

$ vc 'hex "\u{}"'
1 >= '"00"'

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

$ vc 'time() > 1444456789'
1 >= true

%

$ vc 'getenv "This environment variable is almost certainly not defined."'
1 >= "()"

%

$ FOO=foo vc 'getenv "FOO"'
1 >= '"foo"'

%

$ FOO="" vc 'getenv "FOO"'
1 >= '""'

%

$ vc 'half(2)^2'
1 >= 1

%

$ vc '2^half(2)'
1 >= 2

%

$ vc 'time()^0'
1 >= 1

%

$ vc '1^time()'
1 >= 1

%

$ vc 'str time'
1 >= '"time"'

%

$ vc 'bool time'
1 >= true

%

$ vc 'time == time'
1 >= true

%

$ vc 'time == getenv'
1 >= false

%

$ vc '"hey" * 3'
1 >= '"heyheyhey"'

%

$ vc 'print ("THE CAKE IS A LIE\n" * 4)'
1 >> .
THE CAKE IS A LIE
THE CAKE IS A LIE
THE CAKE IS A LIE
THE CAKE IS A LIE

.

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

$ vc '(+) time'
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
1 >> .
.

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

$ vc 'var x = 7; x /= 3; x'
1 >= 2

%

$ vc 'var x = 7; x %= 3; x'
1 >= 1

%

$ vc 'var x = 0; var i = 1;' 'x += i * +((i % 3) * (i % 5) == 0); i += 1; PC -= +(i < 1000)' x
1 >= 233168

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

$ vc '"\u{}".length'
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

$ THREE=3 vc 'getenv("THREE").length'
1 >= 1

%

$ vc 'str().length'
1 >= 0

%

$ vc 'print ""'
1 >= ""

%

$ vc 'print ()'
1 >= ""

%

$ vc 'print "\n"'
1 >> .


.

%

$ vc 'print (1, 2, 3)'
1 >= 123

%

$ vc 'const x = 3; print x'
1 >= 3

%

$ FOO=foo vc 'print getenv "FOO"'
1 >= foo

%

$ FOO=BAR BAR=BAZ vc 'print getenv getenv "FOO"'
1 >= BAZ

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

$ vc 'print 2^2'
1 >= 4

%

$ vc '(+) (time % ())'
1 >= 1

%

$ vc '(+) (getenv % "TERM")'
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

$ vc 'time % ()'
1 >= time

%

$ vc 'time % () == time'
1 >= true

%

$ vc 'getenv % "TERM"'
1 >= '(getenv % "TERM")'

%

$ vc '(getenv % "TERM")() == getenv "TERM"'
1 >= true

%

$ vc 'const csv = join % ","; csv (1, 2, 3)'
1 >= '"1,2,3"'

%

$ vc 'join % (getenv % "TERM")'
1 >= '(join % (getenv % "TERM"))'

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

$ vc 'unbin "0" == "\0"'
1 >= true

%

$ vc 'unbin "01001" == "\t"'
1 >= true

%

$ vc 'unbin "11110000100111111001001010101001" == "\u{1f4a9}"'
1 >= true

%

$ vc 'unhex "0" == "\0"'
1 >= true

%

$ vc 'unhex "01001" == "\x00\x10\x01"'
1 >= true

%

$ vc 'unhex "73776f726466697368"'
1 >= '"swordfish"'

%

$ vc 'b"01000110011011110110111100001010"'
1 >= '"Foo\n"'

%

$ vc 'b"00100000".join(1, 2, 3)'
1 >= '"1 2 3"'

%

$ vc 'x"4E75"'
1 >= '"Nu"'

%

$ vc 'x"73776f726466697368".length'
1 >= 9

%

$ vc 'print rep ()'
1 >= "()"

%

$ vc 'print rep true'
1 >= true

%

$ vc 'print rep (12345)'
1 >= 12345

%

$ vc 'print rep "test"'
1 >= '"test"'

%

$ vc 'print rep "\007\x0a"'
1 >= '"\a\n"'

%

$ vc 'rep "\\ \"" == "\"\\\\ \\\"\""'
1 >= true

%

$ vc 'print rep time'
1 >= time

%

$ vc 'print rep (join % ",")'
1 >= '(join % ",")'

%

$ vc 'print rep (1, 2, 3)'
1 >= "(1, 2, 3)"

%

$ vc 'print rep (join % "1" % "2", join % (getenv % "3"))'
1 >= '((join % ("1", "2")), (join % (getenv % "3")))'

%

$ vc 'typeof ()'
1 >= "()"

%

$ vc 'print typeof true'
1 >= boolean

%

$ vc 'print typeof 0'
1 >= integer

%

$ vc 'print typeof ""'
1 >= string

%

$ vc 'print typeof time'
1 >= function

%

$ vc 'print typeof (join % ",")'
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
