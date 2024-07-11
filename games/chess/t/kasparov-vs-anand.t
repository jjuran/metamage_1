#!/usr/bin/env jtest

# Gary Kasparov vs. Viswanathan Anand
# New York, 1995 (Ruy Lopez)

$ chess-test
0 << '[END]'
1. e4 e5
2. Nf3 Nc6
3. Bb5 a6
4. Ba4 Nf6
5. 0-0 Nxe4
6. d4 b5
7. Bb3 d5
8. dxe5 Be6
9. c3 Nc5
10. Nbd2 d4
11. Ng5 dxc3
12. Nxe6 fxe6
13. bxc3 Qd3
14. Bc2 Qxc3
15. Nb3 Nxb3
16. Bxb3 Nd4
17. Qg4 Qxa1
18. Bxe6 Rd8
19. Bh6 Qc3
20. Bxg7 Qd3
21. Bxh8 Qg6
22. Bf6 Be7
23. Bxe7 Qxg4
24. Bxg4 Kxe7
25. Rc1 c6
26. f4 a5
27. Kf2 a4
28. Ke3 b4
29. Bd1 a3
30. g4 Rd5
31. Rc4 c5
32. Ke4 Rd8
33. Rxc5 Ne6
34. Rd5 Rc8
35. f5 Rc4+
36. Ke3 Nc5
37. g5 Rc1
38. Rd6
v
[END]
1 >> '[END]'
               
        k     v
      R        
    n   ^ ^ ^  
  v            
v       K      
^             ^
    r B        
[END]
