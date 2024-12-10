#!/usr/bin/env jtest

$ vc '[] ver {throw 0}'
1 >= '[]'

$ vc '0 .. 5 ver {}'
1 >= '[]'

$ vc '0 .. 5 ver ...'
1 >= '[1, 2, 3, 4, 5]'

$ vc '0 .. 5 ver { v % 2 }'
1 >= '[1, 3, 5]'

$ vc 'str ("getting disemvoweled makes me kind of mad" ver {not v in "aeiou"})'
1 >= '"gttng dsmvwld mks m knd f md"'

$ vc '[1, "of", "these", "things"] ver {typeof v != string}'
1 >= '[1]'

$ vc '[u8, u16, u32] ver {v.size > 1}'
1 >= '[u16, u32]'

$ vc '"gullible" pop {v == *"l"}'
1 >= 3

$ vc '["egg", "sausage", "bacon"] any { v == "spam" }'
1 >= false

$ vc '["bacon", "egg", "sausage", "spam"] any { v == "spam" }'
1 >= true

$ vc '["spam", "egg", "sausage", "spam"] all { v == "spam" }'
1 >= false

$ vc '["spam" (*) 11] all { v == "spam" }'
1 >= true

$ vc '[] any { v == "spam" }'
1 >= false

$ vc '[] all { v == "spam" }'
1 >= true
