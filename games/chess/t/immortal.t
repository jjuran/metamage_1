#!/usr/bin/env jtest

# Adolf Anderssen vs. Lionel Kieseritzky
# "The Immortal Game"
# London, 1851 (King's Gambit)

$ chess-test
0 << '[END]'
1. e4 e5
2. f4 exf4
3. Bc4 Qh4+
4. Kf1 b5
5. Bxb5 Nf6
6. Nf3 Qh6
7. d3 Nh5
8. Nh4 Qg5
9. Nf5 c6
10. g4 Nf6
11. Rg1 cxb5
12. h4 Qg6
13. h5 Qg5
14. Qf3 Ng8
15. Bxf4 Qf6
16. Nc3 Bc5
17. Nd5 Qxb2
18. Bd6 Bxg1
19. e5 Qxa1+
20. Ke2 Na6
21. Nxg7+ Kd8
22. Qf6+ Nxf6
23. Be7#
v
[END]
1 >> '[END]'
white win
r   b #       r
v     v B v N v
n         n    
  v   N ^     ^
            ^  
      ^        
^   ^   W      
q           b  
[END]
