#!/usr/bin/env jtest

$ chess-test e4 e5 Bc4 Bc5 Nf3 Nf6 e1-g1 v
1 >> '[END]'
r n b q k     r
v v v v   v v v
          n    
    b   v      
    B   ^      
          N    
^ ^ ^ ^   ^ ^ ^
R N B Q   R K  
[END]

$ chess-test e4 e5 Bc4 Bc5 Nf3 Nf6 Kg1 Kg8 v
1 >> '[END]'
r n b q   r k  
v v v v   v v v
          n    
    b   v      
    B   ^      
          N    
^ ^ ^ ^   ^ ^ ^
R N B Q   R K  
[END]

$ chess-test d4 d5 Qd3 Qd6 Be3 Be6 Nc3 Nc6 Kc1 v
1 >> '[END]'
r       k b n r
v v v   v v v v
    n q b      
      v        
      ^        
    N Q B      
^ ^ ^   ^ ^ ^ ^
    K R   B N R
[END]

$ chess-test d4 d5 Qd3 Qd6 Be3 Be6 Nc3 Nc6 Kc1 Kc8 v
1 >> '[END]'
    k r   b n r
v v v   v v v v
    n q b      
      v        
      ^        
    N Q B      
^ ^ ^   ^ ^ ^ ^
    K R   B N R
[END]

$ chess-test e4 e5 Bc4 Bc5 Nf3 Nf6 a4 a5 Ra3 Ra6 0-0 v
1 >> '[END]'
  n b q k     r
  v v v   v v v
r         n    
v   b   v      
^   B   ^      
R         N    
  ^ ^ ^   ^ ^ ^
  N B Q   R K  
[END]

$ chess-test e4 e5 Bc4 Bc5 Nf3 Nf6 a4 a5 Ra3 Ra6 0-0 0-0 v
1 >> '[END]'
  n b q   r k  
  v v v   v v v
r         n    
v   b   v      
^   B   ^      
R         N    
  ^ ^ ^   ^ ^ ^
  N B Q   R K  
[END]

$ chess-test d4 d5 Qd3 Qd6 Be3 Be6 Nc3 Nc6 h4 h5 Rh3 Rh6 0-0-0 v
1 >> '[END]'
r       k b n  
v v v   v v v  
    n q b     r
      v       v
      ^       ^
    N Q B     R
^ ^ ^   ^ ^ ^  
    K R   B N  
[END]

$ chess-test d4 d5 Qd3 Qd6 Be3 Be6 Nc3 Nc6 h4 h5 Rh3 Rh6 0-0-0 0-0-0 v
1 >> '[END]'
    k r   b n  
v v v   v v v  
    n q b     r
      v       v
      ^       ^
    N Q B     R
^ ^ ^   ^ ^ ^  
    K R   B N  
[END]

$ chess-test e4 e5 Bc4 Bc5 Nf3 Nf6 Rg1 Rg8 Rh1 Rh8 0-0 v
1 >= "castling forbidden"

$ chess-test e4 e5 Bc4 Bc5 Nf3 Nf6 Rg1 Rg8 Rh1 Rh8 Kf1 0-0 v
1 >= "castling forbidden"

$ chess-test d4 d5 Qd3 Qd6 Be3 Be6 Nc3 Nc6 Rb1 Rb8 Ra1 Ra8 0-0-0 v
1 >= "castling forbidden"

$ chess-test d4 d5 Qd3 Qd6 Be3 Be6 Nc3 Nc6 Rb1 Rb8 Ra1 Ra8 Kd1 0-0-0 v
1 >= "castling forbidden"

$ chess-test e4 e5 Bc4 Bc5 Nf3 Nf6 Kf1 Kf8 Ke1 Ke8 0-0 v
1 >= "castling forbidden"

$ chess-test e4 e5 Bc4 Bc5 Nf3 Nf6 Kf1 Kf8 Ke1 Ke8 Kf1 0-0 v
1 >= "castling forbidden"

$ chess-test d4 d5 Qd3 Qd6 Be3 Be6 Nc3 Nc6 Kd1 Kd8 Ke1 Ke8 0-0-0 v
1 >= "castling forbidden"

$ chess-test d4 d5 Qd3 Qd6 Be3 Be6 Nc3 Nc6 Kd1 Kd8 Ke1 Ke8 Kd1 0-0-0 v
1 >= "castling forbidden"
