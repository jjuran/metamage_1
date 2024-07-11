#!/usr/bin/env jtest

$ chess-test g4 e5 f4 Qh4 v
1 >> '[END]'
black win
r n b   W b n r
v v v v   v v v
               
        v      
          ^ ^ q
               
^ ^ ^ ^ ^     ^
R N B Q # B N R
[END]

$ chess-test 1. a3 g5 2. e4 f5 3. Qh5 v
1 >> '[END]'
white win
r n b q # b n r
v v v v v     v
               
          v v Q
        ^      
^              
  ^ ^ ^   ^ ^ ^
R N B   W B N R
[END]

$ chess-test e4 e5 Bc4 Bc5 Qh5 Nf6 Qf7 v
1 >> '[END]'
white win
r n b q #     r
v v v v   Q v v
          n    
    b   v      
    B   ^      
               
^ ^ ^ ^   ^ ^ ^
R N B   W   N R
[END]
