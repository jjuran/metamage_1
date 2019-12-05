#!/usr/bin/env jtest

$ vc 'Iter.min []'
1 >= '()'

%

$ vc 'Iter.min [123]'
1 >= 123

%

$ vc 'Iter.min [3, 1, 4, 1, 5, 9, 2, 6, 5]'
1 >= 1

%

$ vc 'Iter.min ["Voila! In view, a humble Vaudevillian veteran" / *" "]'
1 >= '"In"'

%

$ vc 'Iter.max []'
1 >= '()'

%

$ vc 'Iter.max [123]'
1 >= 123

%

$ vc 'Iter.max [3, 1, 4, 1, 5, 9, 2, 6, 5]'
1 >= 9

%

$ vc 'Iter.max ["Voila! In view, a humble Vaudevillian veteran" / *" "]'
1 >= '"view,"'
