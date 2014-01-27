#!/usr/bin/env jtest

$ echo _{a,}{1,}_

1 >= "_a1_ _a_ _1_ __"

%%

$ echo {a,b}{1,2}

1 >= "a1 a2 b1 b2"

%%

$ echo {a,b{c,d}}{1,2}

1 >= "a1 a2 bc1 bc2 bd1 bd2"

%%

$ echo {foo}{bar}

1 >= "{foo}{bar}"

