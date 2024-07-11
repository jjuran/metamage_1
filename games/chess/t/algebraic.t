#!/usr/bin/env jtest

$ chess-test Nc3 v
1 >> '[END]'
r n b q k b n r
v v v v v v v v
               
               
               
    N          
^ ^ ^ ^ ^ ^ ^ ^
R   B Q K B N R
[END]

$ chess-test e3 v
1 >> '[END]'
r n b q k b n r
v v v v v v v v
               
               
               
        ^      
^ ^ ^ ^   ^ ^ ^
R N B Q K B N R
[END]

$ chess-test e4 v
1 >> '[END]'
r n b q k b n r
v v v v v v v v
               
               
        ^      
               
^ ^ ^ ^   ^ ^ ^
R N B Q K B N R
[END]

$ chess-test e4 d6 v
1 >> '[END]'
r n b q k b n r
v v v   v v v v
      v        
               
        ^      
               
^ ^ ^ ^   ^ ^ ^
R N B Q K B N R
[END]

$ chess-test e4 d5 v
1 >> '[END]'
r n b q k b n r
v v v   v v v v
               
      v        
        ^      
               
^ ^ ^ ^   ^ ^ ^
R N B Q K B N R
[END]

$ chess-test e4 d5 exd5 v
1 >> '[END]'
r n b q k b n r
v v v   v v v v
               
      ^        
               
               
^ ^ ^ ^   ^ ^ ^
R N B Q K B N R
[END]

$ chess-test e4 d5 exd v
1 >> '[END]'
r n b q k b n r
v v v   v v v v
               
      ^        
               
               
^ ^ ^ ^   ^ ^ ^
R N B Q K B N R
[END]

$ chess-test f4 e5 fxe d5 e4 Qd6 exd
1 >= "ambiguous source inference"
