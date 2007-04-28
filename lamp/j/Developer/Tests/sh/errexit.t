#!/usr/bin/env jtest

$ sh -c 'set -e; echo pretty; false; echo bad'

? 1

1 >= pretty

%%

$ sh -c 'set -e; set +e; false; echo good'

1 >= good

%%

$ sh /dev/fd/3

? 1

3 << 'END'
set -e
echo pretty
false
echo bad
END

1 >= pretty

%%

$ sh /dev/fd/3

? 0

3 << 'END'
set -e
set +e
false
echo good
END

1 >= good

