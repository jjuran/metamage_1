#!/usr/bin/env jtest

$ vx -e 'const sec = x"00" * 32; print rep ed25519-publickey sec'
1 >= 'x"3b6a27bcceb6a42d62a3a8d02a6f0d73653215771de243a63ac048a18b59da29"'

%

$ vx -e 'const sec = packed( (0 -> 32) map byte ); print rep ed25519-publickey sec'
1 >= 'x"03a107bff3ce10be1d70dd18e74bc09967e4d6309ba50d5f1ddc8664125531b8"'

%

$ vx -e 'const sec = x"00" * 32; print rep ed25519-sign( sec, "Hello world" )[ 0 -> 32 ]'
1 >= 'x"b307c9b078adcbd5e9b64a7fec5673490e39973eb5b2f76f2b0a357b10b5e9c2"'

%

$ vx -e 'const sec = x"00" * 32; print rep ed25519-sign( sec, "Hello world" )[ 32 -> 64 ]'
1 >= 'x"f393eb3472116bb0a6007dc833648e9e3a8b6cac049c8380657a9064161b2c04"'

%

$ vx -e 'var k = x"00" * 32; var sig = ed25519-sign( k, "!" ); print rep ed25519-verify( ed25519-publickey k, "!", sig )'
1 >= true

%

$ vx -e 'var k = x"00" * 32; var sig = ed25519-sign( k, "!" ); print rep ed25519-verify( ed25519-publickey k, "!", k*2 )'
1 >= false

%

$ vx -e 'var k = x"00" * 32; var sig = ed25519-sign( k, "!" ); print rep ed25519-verify( ed25519-publickey k, "?", sig )'
1 >= false

%

$ vx -e 'var k = x"00" * 32; var sig = ed25519-sign( k, "!" ); print rep ed25519-verify( k, "!", sig )'
1 >= false

%

$ vx -e 'print rep {ed25519-publickey (x"00" * 32)}'
1 >= '{x"3b6a27bcceb6a42d62a3a8d02a6f0d73653215771de243a63ac048a18b59da29"}'
