#!/usr/bin/env jtest

$ vc 'def f () {123}; f()'
1 >= 123

%

$ vc 'def f (x) {x^2}; f 5'
1 >= 25

%

$ vc 'def f (x, y) {x * y}; f (3, 4)'
1 >= 12

%

$ vc 'def f (x: int, y: int) {x^2 + y}; f (3, 4)'
1 >= 13

%

$ vc 'def f (x, y: ...) {x, [y]}; f 1'
1 >= '(1, [])'

%

$ vc 'def f (x, y: ...) {x, [y]}; f (1, 2)'
1 >= '(1, [2])'

%

$ vc 'def f () {123}; try {f 123} catch {"nope"}'
1 >= '"nope"'

%

$ vc 'def f (x) {x}; try {f()} catch {"nope"}'
1 >= '"nope"'

%

$ vc 'def f (x) {x}; try {f(1, 2)} catch {"nope"}'
1 >= '"nope"'

%

$ vc 'def f (x: int, y: int) {x^2 + y}; try {f (3, 4, 5)} catch {"nope"}'
1 >= '"nope"'

%

$ vc 'def f (x: int, y: int) {x^2 + y}; try {f (3.5, 4)} catch {"nope"}'
1 >= '"nope"'

%

$ vc 'def f (x, y: ...) {x, [y]}; try {f()} catch {"nope"}'
1 >= '"nope"'

%

$ vc 'export def f () {...}'
1 >= '(export f)'

%

$ vc 'def f (x: int, y) {x + y}; f.parameter-names'
1 >= '[x, y]'

%

$ vc 'def f (x: int) {x + 4}; f.parameter-names'
1 >= '[x]'

%

$ vc 'def f () {7}; f.parameter-names'
1 >= '[]'

%

$ vc 'def f {7}; f.parameter-names'
1 >= '()'

%

$ vc 'const f = lambda (x, y: int) {x + y}; f.parameter-names'
1 >= '[x, y]'

%

$ vc 'const f = lambda (x) {x + 4}; f.parameter-names'
1 >= '[x]'

%

$ vc 'const f = lambda () {7}; f.parameter-names'
1 >= '[]'

%

$ vc 'const f = lambda {7}; f.parameter-names'
1 >= '()'
