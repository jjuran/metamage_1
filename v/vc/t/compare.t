#!/usr/bin/env jtest

$ vc '() < (), () <= (), () >= (), () > (), () <=> ()'
1 >= '(false, true, true, false, 0)'

$ vc '(1, 6) <=> (1, 6)'
1 >= 0

$ vc "(2, 'b') <=> (-2, 'b')"
1 >= 1

$ vc '("Mac", "OS", 8) <=> ("Mac", "OS", 9)'
1 >= -1

$ vc 'try { ("Mac", "OS", 9) <=> ("Mac", "OS", "X") } catch { "nope" }'
1 >= '"nope"'
