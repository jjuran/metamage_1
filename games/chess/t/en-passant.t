#!/usr/bin/env jtest

$ chess-test e4 d5 exd5 e5 dxe6 e.p. v
1 >> '[END]'
r n b q k b n r
v v v     v v v
        ^      
               
               
               
^ ^ ^ ^   ^ ^ ^
R N B Q K B N R
[END]

$ chess-test e4 d5 exd5 e5 a3 a6 dxe6 v
1 >= "move failure"

$ chess-test e4 e5 Ba6 Ba3 Nf3 Nf6 b4 c5 bxc5 b5 cxb6 e.p. v
1 >> '[END]'
r n b q k     r
v     v   v v v
B ^       n    
        v      
        ^      
b         N    
^   ^ ^   ^ ^ ^
R N B Q K     R
[END]

$ chess-test e4 e5 Ba6 Ba3 Nf3 Nf6 b4 c5 bxc5 b5 0-0 0-0 cxb6 v
1 >= "move failure"

$ chess-test e4 d5 exd5 a6 d4 e5 dxe5 v
1 >> '[END]'
r n b q k b n r
  v v     v v v
v              
      ^ ^      
               
               
^ ^ ^     ^ ^ ^
R N B Q K B N R
[END]

$ chess-test e4 d5 exd5 a6 d4 e5 dxe6 e.p. v
1 >> '[END]'
r n b q k b n r
  v v     v v v
v       ^      
               
      ^        
               
^ ^ ^     ^ ^ ^
R N B Q K B N R
[END]

$ chess-test f4 g5 f5 a6 d4 a5 Bf4 gxf4 e4 fxe3 e.p. v
1 >> '[END]'
r n b q k b n r
  v v v v v   v
               
v         ^    
      ^        
        v      
^ ^ ^       ^ ^
R N   Q K B N R
[END]
