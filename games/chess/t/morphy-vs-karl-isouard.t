#!/usr/bin/env jtest

# Paul Morphy vs. Duke Karl of Braunschweig & Count Isouard
# Paris (at the Opera), 1958 (Philidor Defense)

$ chess-test
0 << '[END]'
1. e4 e5
2. Nf3 d6
3. d4 Bg4
4. dxe5 Bxf3
5. Qxf3 dxe5
6. Bc4 Nf6
7. Qb3 Qe7
8. Nc3 c6
9. Bg5 b5
10. Nxb5 cxb5
11. Bxb5+ Nbd7
12. 0-0-0 Rd8
13. Rxd7 Rxd7
14. Rd1 Qe6
15. Bxd7+ Nxd7
16. Qb8+ Nxb8
17. Rd8#
v
[END]
1 >> '[END]'
white win
  n   R # b   r
v         v v v
        q      
        v   B  
        ^      
               
^ ^ ^     ^ ^ ^
    W          
[END]
